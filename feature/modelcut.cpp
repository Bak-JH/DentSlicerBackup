#include "modelcut.h"


modelcut::modelcut(){
    numPoints = 0;
}

void modelcut::bisectModel(Mesh* mesh, Plane plane, Mesh* leftMesh, Mesh* rightMesh){
    // do bisecting mesh
    leftMesh->faces.reserve(mesh->faces.size()*2);
    leftMesh->vertices.reserve(mesh->faces.size()*2);
    rightMesh->faces.reserve(mesh->faces.size()*2);
    rightMesh->vertices.reserve(mesh->faces.size()*2);

    foreach (MeshFace mf, mesh->faces){
        bool faceLeftToPlane = true;
        for (int vn=0; vn<3; vn++){
            MeshVertex mv = mesh->vertices[mf.mesh_vertex[vn]];
            if (!isLeftToPlane(plane, mv.position)) // if one vertex is left to plane, append to left vertices part
                faceLeftToPlane = false;
        }
        if (faceLeftToPlane)
            leftMesh->addFace(mesh->vertices[mf.mesh_vertex[0]].position, mesh->vertices[mf.mesh_vertex[1]].position, mesh->vertices[mf.mesh_vertex[2]].position);
        else
            rightMesh->addFace(mesh->vertices[mf.mesh_vertex[0]].position, mesh->vertices[mf.mesh_vertex[1]].position, mesh->vertices[mf.mesh_vertex[2]].position);
    }

    qDebug() << "done bisect";

    leftMesh->connectFaces();
    rightMesh->connectFaces();
    qDebug() << "done connecting";
}

bool isLeftToPlane(Plane plane, QVector3D position){
    // determine if position is left to plane or not
    if(position.distanceToPlane(plane[0],plane[1],plane[2])==0){
        qDebug() << "distance to plane 0";
    }
    if(position.distanceToPlane(plane[0],plane[1],plane[2])>0)
        return false;
    return true;
}

void modelcut::generatePlane(Qt3DCore::QEntity* targetEntity){

    if (cuttingPoints.size()<3){
        qDebug()<<"Error: There is not enough vectors to render a plane.";
        QCoreApplication::quit();
        return;
    }

    QVector3D v1;
    QVector3D v2;
    QVector3D v3;
    v1=cuttingPoints[cuttingPoints.size()-3];
    v2=cuttingPoints[cuttingPoints.size()-2];
    v3=cuttingPoints[cuttingPoints.size()-1];
        planeMaterial = new Qt3DExtras::QPhongAlphaMaterial();
        planeMaterial->setAmbient(QColor(QRgb(0xF4F4F4)));
        planeMaterial->setDiffuse(QColor(QRgb(0xF4F4F4)));
        planeMaterial->setSpecular(QColor(QRgb(0xF4F4F4)));
        planeMaterial->setAlpha(0.5f);


        //To manipulate plane color, change the QRgb(0x~~~~~~).
        QVector3D world_origin(0,0,0);
        QVector3D original_normal(0,1,0);
        QVector3D desire_normal(QVector3D::normal(v1,v2,v3)); //size=1
        float angle = qAcos(QVector3D::dotProduct(original_normal,desire_normal))*180/M_PI;
        QVector3D crossproduct_vector(QVector3D::crossProduct(original_normal,desire_normal));

        for (int i=0;i<2;i++){
            clipPlane[i]=new Qt3DExtras::QPlaneMesh();
            clipPlane[i]->setHeight(30.0);
            clipPlane[i]->setWidth(30.0);

            planeTransform[i]=new Qt3DCore::QTransform();
            planeTransform[i]->setScale(2.0f);
            planeTransform[i]->setRotation(QQuaternion::fromAxisAndAngle(crossproduct_vector, angle+180*i));
            planeTransform[i]->setTranslation(desire_normal*(-world_origin.distanceToPlane(v1,v2,v3)));

            qDebug() << "before target generation";
            moveToThread(targetEntity->thread());
            qDebug() << thread();
            qDebug() << targetEntity->thread();

            planeEntity[i] = new Qt3DCore::QEntity(targetEntity);
            planeEntity[i]->addComponent(clipPlane[i]);
            planeEntity[i]->addComponent(planeTransform[i]);
            planeEntity[i]->addComponent(planeMaterial);
            qDebug() << "after target generation";
        }
}

void modelcut::addCuttingPoint(Qt3DCore::QEntity* targetEntity, QVector3D v){
    cuttingPoints.push_back(v);
    sphereMesh[numPoints] = new Qt3DExtras::QSphereMesh;
    sphereMesh[numPoints]->setRadius(1);
    sphereTransform[numPoints] = new Qt3DCore::QTransform;
    sphereTransform[numPoints]->setTranslation(v);

    sphereMaterial[numPoints] = new Qt3DExtras::QPhongMaterial();
    sphereMaterial[numPoints]->setAmbient(QColor(QRgb(0x0049FF)));
    sphereMaterial[numPoints]->setDiffuse(QColor(QRgb(0x0049FF)));
    sphereMaterial[numPoints]->setSpecular(QColor(QRgb(0x0049FF)));
    sphereMaterial[numPoints]->setShininess(0.0f);

    sphereEntity[numPoints] = new Qt3DCore::QEntity(targetEntity);
    sphereEntity[numPoints]->addComponent(sphereMesh[numPoints]);
    sphereEntity[numPoints]->addComponent(sphereTransform[numPoints]);
    sphereEntity[numPoints]->addComponent(sphereMaterial[numPoints]);
    numPoints++;

}

// interpolate between two random closed same oriented contours
void interpolate(Mesh* mesh, Path3D contour1, Path3D contour2){
    Path3D bigger = contour2;
    Path3D smaller = contour1;
    if (contour1.size() > contour2.size()){
        bigger = contour1;
        smaller = contour2;
    }

    int prev_min_distance_mv_idx = -1;
    int min_distance_mv_idx;
    float min_distance = 99999;

    for (int ct1_idx =0; ct1_idx<bigger.size(); ct1_idx++){
        MeshVertex bigger_mv = bigger[ct1_idx];
        min_distance_mv_idx = 0;
        min_distance = bigger_mv.position.distanceToPoint(smaller[min_distance_mv_idx].position);
        for (int ct2_idx=0; ct2_idx<smaller.size();ct2_idx++){
            float cur_distance = bigger_mv.position.distanceToPoint(smaller[ct2_idx].position);
            if (cur_distance < min_distance){
                min_distance_mv_idx = ct2_idx;
                min_distance = cur_distance;
            }
        }

        // add face to mesh
        if (prev_min_distance_mv_idx == min_distance_mv_idx){
            // add Face from bigger to smaller (smaller 1 bigger 2)
            mesh->addFace(bigger[ct1_idx].position, bigger[(ct1_idx+1)%bigger.size()].position, smaller[min_distance_mv_idx].position);
        } else {
            // add face from smaller to bigger (bigger 1 smaller 2)
            mesh->addFace(smaller[prev_min_distance_mv_idx].position,  bigger[ct1_idx].position, smaller[min_distance_mv_idx].position);
        }
        prev_min_distance_mv_idx = min_distance_mv_idx;
    }
}

void cutAway(Mesh* leftMesh, Mesh* rightMesh, Mesh* mesh, vector<QVector3D> cuttingPoints, int cutFillMode){
    Path contour; // real cutting points in intpoint form
    Path3D cuttingContour; // real cutting points line qvector3d form
    Paths3D cuttingEdges;
    int numPoints = cuttingPoints.size();

    leftMesh->faces.reserve(mesh->faces.size()*2);
    leftMesh->vertices.reserve(mesh->faces.size()*2);
    rightMesh->faces.reserve(mesh->faces.size()*2);
    rightMesh->vertices.reserve(mesh->faces.size()*2);

    for (int i=0; i<numPoints; i++){
        QVector3D cuttingPoint =cuttingPoints[i];
        contour.push_back(IntPoint(cuttingPoint.x()*scfg->resolution
                                   ,cuttingPoint.y()*scfg->resolution));
        MeshVertex temp_mv;
        temp_mv.position = QVector3D(cuttingPoint.x(), cuttingPoint.y(), cuttingPoint.z());
        cuttingContour.push_back(temp_mv);
    }

    Path IntPoints;
    IntPoints.reserve(mesh->vertices.size()*2);
    for (MeshVertex mv : mesh->vertices){
        IntPoints.push_back(IntPoint(mv.position.x()*scfg->resolution, mv.position.y()*scfg->resolution));
    }
    mesh->convexHull = getConvexHull(&IntPoints);


    //freecut intersection detect from here

    bool intersectionExists = false;
    bool cpInsideHull = false;
    bool cpOutsideHull = false;

    // saves entering position of cp
    IntPoint enterIP;
    IntPoint exitIP;

    float enter_min_distance = 99999;
    float exit_min_distance = 99999;
    MeshVertex enterIntersection;
    MeshVertex exitIntersection;
    Path contour_copy;
    Path3D cuttingContour_copy;

    // find enter IP , exitIP
    for (int i=0; i<contour.size(); i++){
        if (PointInPolygon(contour[i], mesh->convexHull)){ // inside of hull
            if (!cpInsideHull && !PointInPolygon(contour[(i-1)%contour.size()], mesh->convexHull)){
                cpInsideHull = true;
                enterIP = contour[(i-1)%contour.size()];
                //cuttingContour_copy.push_back(cuttingContour[(i-1)%cuttingContour.size()]);
                contour_copy.push_back(contour[(i-1)%contour.size()]);
            }
            cuttingContour_copy.push_back(cuttingContour[i]);
            contour_copy.push_back(contour[i]);
        } else { // outside of hull
            if (!cpOutsideHull && PointInPolygon(contour[(i-1)%contour.size()], mesh->convexHull)){
                cpOutsideHull = true;
                exitIP = contour[i];
                //cuttingContour_copy.push_back(cuttingContour[i]);
                contour_copy.push_back(contour[i]);
            }
        }
    }

    qDebug() << "found exit, enter ip" <<exitIP.X << enterIP.X;

    cuttingContour = cuttingContour_copy;
    Path contour_all = contour;
    contour = contour_copy;

    if (cpInsideHull && cpOutsideHull)
        intersectionExists = true;

    // find intersection if it exists

    // freecut detection done till here


    Paths3D innerContours, outerContours;

    foreach (MeshFace mf, mesh->faces){
        bool faceLeftToPlane = false;
        bool faceRightToPlane = false;

        Path3D leftContour;
        Path3D rightContour;

        for (int vn=0; vn<3; vn++){
            MeshVertex mv = mesh->vertices[mf.mesh_vertex[vn]];

            if (PointInPolygon(IntPoint(mv.position.x()*scfg->resolution, mv.position.y()*scfg->resolution), contour_all)){
                faceLeftToPlane = true;
                leftContour.push_back(mv);
            } else {
                faceRightToPlane = true;
                rightContour.push_back(mv);
            }
        }

        if (faceLeftToPlane && faceRightToPlane){ // cutting edge

            if (intersectionExists){
                // calculate intersection point (least distance intpoint from enterIP, exitIPs)
                QVector3D mv = mesh->vertices[mf.mesh_vertex[0]].position;
                QVector3D commv = (mesh->vertices[mf.mesh_vertex[0]].position +
                        mesh->vertices[mf.mesh_vertex[1]].position +
                        mesh->vertices[mf.mesh_vertex[2]].position)/3;

                IntPoint intmv = IntPoint(mv.x()*scfg->resolution, mv.y()*scfg->resolution);


                bool faceLeftToPlane_int = false;
                bool faceRightToPlane_int = false;

                // check if vertex earned from intersection
                for (int vn=0; vn<3; vn++){
                    QVector3D mv = mesh->vertices[mf.mesh_vertex[vn]].position;
                    QVector3D enterQV3 = QVector3D(((float)enterIP.X)/((float)scfg->resolution), ((float)enterIP.Y)/((float)scfg->resolution), 0);
                    Plane plane;
                    plane.push_back(enterQV3);
                    plane.push_back(cuttingContour[0].position);
                    plane.push_back(QVector3D(enterQV3.x(),enterQV3.y(),1));
                    if (isLeftToPlane(plane, mv)) // if one vertex is left to plane, append to left vertices part
                        faceLeftToPlane_int = true;
                    else {
                        faceRightToPlane_int = true;
                    }
                }

                if (faceLeftToPlane_int && faceRightToPlane_int){ // current mesh face is earned from intersection
                    // check distance from enterIP
                    float cur_distance = pointDistance(enterIP, intmv);
                    if (cur_distance < enter_min_distance){
                        enter_min_distance = cur_distance;
                        enterIntersection = commv;
                        qDebug() << "found enterIntersection : " << mv <<enterIP.X << enterIP.Y << enter_min_distance;
                    }
                }

                faceLeftToPlane_int = false;
                faceRightToPlane_int = false;

                // check if vertex earned from intersection
                for (int vn=0; vn<3; vn++){
                    QVector3D mv = mesh->vertices[mf.mesh_vertex[vn]].position;
                    QVector3D exitQV3 = QVector3D(((float)exitIP.X)/((float)scfg->resolution), ((float)exitIP.Y)/((float)scfg->resolution), 0);
                    Plane plane;
                    plane.push_back(exitQV3);
                    plane.push_back(cuttingContour[cuttingContour.size()-1].position);
                    plane.push_back(QVector3D(exitQV3.x(),exitQV3.y(),1));
                    if (isLeftToPlane(plane, mv)) // if one vertex is left to plane, append to left vertices part
                        faceLeftToPlane_int = true;
                    else {
                        faceRightToPlane_int = true;
                    }
                }

                if (faceLeftToPlane_int && faceRightToPlane_int){ // current mesh face is earned from intersection
                    // check distance from exitIP
                    float cur_distance = pointDistance(exitIP, intmv);
                    if (cur_distance < exit_min_distance){
                        exit_min_distance = cur_distance;
                        exitIntersection = commv;
                        qDebug() << "found exitIntersection : "<< mv << exitIP.X << exitIP.Y << exit_min_distance;
                    }
                }
            }

            if (leftContour.size() == 2){
                innerContours.push_back(leftContour);
            } else if (rightContour.size() == 2) {
                outerContours.push_back(rightContour);
            }
        } else if (faceLeftToPlane){
            leftMesh->addFace(mesh->vertices[mf.mesh_vertex[0]].position, mesh->vertices[mf.mesh_vertex[1]].position, mesh->vertices[mf.mesh_vertex[2]].position);
        } else if (faceRightToPlane){
            rightMesh->addFace(mesh->vertices[mf.mesh_vertex[0]].position, mesh->vertices[mf.mesh_vertex[1]].position, mesh->vertices[mf.mesh_vertex[2]].position);
        }
    }
    qDebug() << "exit, enter intersection : " << exitIntersection.position << enterIntersection.position;
    //Paths3D innerContours_c = contourConstruct3D(innerContours);
    //Paths3D outerContours_c = contourConstruct3D(outerContours);


    // find maximal, minimal position of cuttingContour
    Path3D cuttingContour_u;
    Path3D cuttingContour_l;

    for (int ctc_idx=0; ctc_idx<cuttingContour.size(); ctc_idx++){
        cuttingContour_u.push_back(cuttingContour[ctc_idx]);
        cuttingContour_l.push_back(cuttingContour[ctc_idx]);
    }

    for (int ctc_idx=0; ctc_idx<cuttingContour.size(); ctc_idx++){
        MeshVertex qv = cuttingContour[ctc_idx];

        // find maximal position
        for (MeshFace mf : mesh->faces){
            // if projection of mf to xy coordinates contains qv and upper than current qv's position, get qv
            MeshVertex mfv1 = mesh->idx2MV(mf.mesh_vertex[0]);
            MeshVertex mfv2 = mesh->idx2MV(mf.mesh_vertex[1]);
            MeshVertex mfv3 = mesh->idx2MV(mf.mesh_vertex[2]);
            Path mf_projection;
            mf_projection.push_back(IntPoint(mfv1.position.x()*scfg->resolution, mfv1.position.y()*scfg->resolution));
            mf_projection.push_back(IntPoint(mfv2.position.x()*scfg->resolution, mfv2.position.y()*scfg->resolution));
            mf_projection.push_back(IntPoint(mfv3.position.x()*scfg->resolution, mfv3.position.y()*scfg->resolution));
            if (PointInPolygon(IntPoint(qv.position.x()*scfg->resolution, qv.position.y()*scfg->resolution), mf_projection)){
                qDebug() << "point in polygon";
                // get intersection point and compare its height
                QVector3D face_normal = QVector3D::normal(mfv1.position, mfv2.position, mfv3.position);
                float height = (face_normal.x()*mfv1.position.x() + face_normal.y()*mfv1.position.y() + face_normal.z()*mfv1.position.z()
                        - face_normal.x()*qv.position.x() - face_normal.y()*qv.position.y())/face_normal.z();

                //qDebug() << "height : " <<height << mfv1.position.z() << mfv2.position.z() << mfv3.position.z();
                if (height > cuttingContour_u[ctc_idx].position.z()){
                    cuttingContour_u[ctc_idx].position = QVector3D(cuttingContour_u[ctc_idx].position.x(), cuttingContour_u[ctc_idx].position.y(), height);
                }
                if (height <= cuttingContour_l[ctc_idx].position.z()) {
                    cuttingContour_l[ctc_idx].position = QVector3D(cuttingContour_l[ctc_idx].position.x(), cuttingContour_l[ctc_idx].position.y(), height);
                }
            }
        }
    }

    qDebug() << "intersections : " << enterIntersection.position << exitIntersection.position;


    if (intersectionExists){ // insert exit intersection
        cuttingContour_u.insert(cuttingContour_u.begin(), enterIntersection);
        cuttingContour_l.insert(cuttingContour_l.begin(), enterIntersection);
    }


    if (intersectionExists){ // insert exit intersection
        cuttingContour_u.push_back(exitIntersection);
        cuttingContour_l.push_back(exitIntersection);
    }

    qDebug() << "upper";
    for (int i=0; i<cuttingContour_u.size(); i++){
        qDebug() << i << cuttingContour_u[i].position;
    }

    qDebug() << "lower";
    for (int i=0; i<cuttingContour_l.size(); i++){
        qDebug() << i << cuttingContour_l[i].position;
    }

    //qDebug() << "constructed inner outer contours : " << innerContours_c.size() << outerContours_c.size();

    QMultiHash<int, MeshVertex> cuttingContourConnections_u;
    QMultiHash<int, MeshVertex> cuttingContourConnections_l;

    for (Path3D innerContour : innerContours){ // connect inner contours
        qDebug() << "inner Contour size : " << innerContour.size();
        Path3D* selectedCuttingContour;
        QMultiHash<int, MeshVertex>* selectedCuttingContourConnections;
        if (abs(innerContour[0].position.z()-cuttingContour_u[1].position.z()) < abs(innerContour[0].position.z()-cuttingContour_l[1].position.z())){
            selectedCuttingContour = &cuttingContour_u;
            selectedCuttingContourConnections = &cuttingContourConnections_u;
        } else {
            selectedCuttingContour = &cuttingContour_l;
            selectedCuttingContourConnections = &cuttingContourConnections_l;
        }

        float min_distance = 99999;
        int min_distance_cmv_idx;
        // find min distance cutting point and connect it
        for (int cmv_idx=0; cmv_idx<selectedCuttingContour->size(); cmv_idx++){
            MeshVertex cmv = (*selectedCuttingContour)[cmv_idx];
            float cur_distance = abs(cmv.position.distanceToPoint(innerContour[0].position));
            if (cur_distance < min_distance){
                min_distance = cur_distance;
                min_distance_cmv_idx = cmv_idx;
            }
        }
        MeshVertex in1 = innerContour[0];
        MeshVertex in2 = innerContour[1];

        if (!intersectionExists || cutFillMode==2){
            leftMesh->addFace(in1.position, in2.position, (*selectedCuttingContour)[min_distance_cmv_idx].position);
            leftMesh->addFace(in2.position, in1.position, (*selectedCuttingContour)[min_distance_cmv_idx].position);
        }
        selectedCuttingContourConnections->insert(min_distance_cmv_idx, in1);
        selectedCuttingContourConnections->insert(min_distance_cmv_idx, in2);
        qDebug() << "after insertion" << min_distance_cmv_idx << selectedCuttingContourConnections->values(min_distance_cmv_idx).size();
    }


    qDebug() << "cuttingContour_u size : " << cuttingContour_u.size();

    if (!intersectionExists || cutFillMode==2){
    for (int c_idx1=0; c_idx1<cuttingContour_u.size(); c_idx1++){
        for (int c_idx2=0; c_idx2<cuttingContour_u.size(); c_idx2++){
            if (c_idx1 == c_idx2)
                continue;
            QList<MeshVertex> cpts1 = cuttingContourConnections_u.values(c_idx1);
            QList<MeshVertex> cpts2 = cuttingContourConnections_u.values(c_idx2);
            foreach(MeshVertex cpt1 , cpts1){
                foreach (MeshVertex cpt2 , cpts2){
                    if (cpt1.position == cpt2.position){ // if contains vertex in common
                        leftMesh->addFace(cuttingContour_u[c_idx1].position, cuttingContour_u[c_idx2].position, cpt1.position);
                        leftMesh->addFace(cuttingContour_u[c_idx2].position, cuttingContour_u[c_idx1].position, cpt1.position);
                    }
                }
            }
        }
    }
    }

    if (!intersectionExists || cutFillMode==2){
    for (int c_idx1=0; c_idx1<cuttingContour_l.size(); c_idx1++){
        for (int c_idx2=0; c_idx2<cuttingContour_l.size(); c_idx2++){
            if (c_idx1 == c_idx2)
                continue;
            QList<MeshVertex> cpts1 = cuttingContourConnections_l.values(c_idx1);
            QList<MeshVertex> cpts2 = cuttingContourConnections_l.values(c_idx2);
            foreach(MeshVertex cpt1 , cpts1){
                foreach (MeshVertex cpt2 , cpts2){
                    if (cpt1.position == cpt2.position){ // if contains vertex in common
                        leftMesh->addFace(cuttingContour_l[c_idx1].position, cuttingContour_l[c_idx2].position, cpt1.position);
                        leftMesh->addFace(cuttingContour_l[c_idx2].position, cuttingContour_l[c_idx1].position, cpt1.position);
                    }
                }
            }
        }
    }
    }


    cuttingContourConnections_u.clear();
    cuttingContourConnections_l.clear();

    for (Path3D outerContour : outerContours){ // connect inner contours
        qDebug() << "outer Contour size : " << outerContour.size();
        Path3D* selectedCuttingContour;
        QMultiHash<int, MeshVertex>* selectedCuttingContourConnections;
        if (abs(outerContour[0].position.z()-cuttingContour_u[1].position.z()) < abs(outerContour[0].position.z()-cuttingContour_l[1].position.z())){
            selectedCuttingContour = &cuttingContour_u;
            selectedCuttingContourConnections = &cuttingContourConnections_u;
        } else {
            selectedCuttingContour = &cuttingContour_l;
            selectedCuttingContourConnections = &cuttingContourConnections_l;
        }

        float min_distance = 99999;
        int min_distance_cmv_idx;
        // find min distance cutting point and connect it
        for (int cmv_idx=0; cmv_idx<selectedCuttingContour->size(); cmv_idx++){
            MeshVertex cmv = (*selectedCuttingContour)[cmv_idx];
            float cur_distance = abs(cmv.position.distanceToPoint(outerContour[0].position));
            if (cur_distance < min_distance){
                min_distance = cur_distance;
                min_distance_cmv_idx = cmv_idx;
            }
        }
        MeshVertex on1 = outerContour[0];
        MeshVertex on2 = outerContour[1];
        if (!intersectionExists || cutFillMode==2){
            rightMesh->addFace(on1.position, on2.position, (*selectedCuttingContour)[min_distance_cmv_idx].position);
            rightMesh->addFace(on2.position, on1.position, (*selectedCuttingContour)[min_distance_cmv_idx].position);
        }
        selectedCuttingContourConnections->insert(min_distance_cmv_idx, on1);
        selectedCuttingContourConnections->insert(min_distance_cmv_idx, on2);
        if (abs(outerContour[0].position.z()-cuttingContour_u[1].position.z()) < abs(outerContour[0].position.z()-cuttingContour_l[1].position.z())){
            qDebug() << "after insertion" << min_distance_cmv_idx << cuttingContourConnections_u.values(min_distance_cmv_idx).size() << on1.position << on2.position;
        }
    }


    qDebug() << "cuttingContour_u size : " << cuttingContour_u.size();

    if (!intersectionExists || cutFillMode==2){
    for (int c_idx1=0; c_idx1<cuttingContour_u.size(); c_idx1++){
        for (int c_idx2=0; c_idx2<cuttingContour_u.size(); c_idx2++){
            if (c_idx1 == c_idx2)
                continue;
            QList<MeshVertex> cpts1 = cuttingContourConnections_u.values(c_idx1);
            QList<MeshVertex> cpts2 = cuttingContourConnections_u.values(c_idx2);
            foreach(MeshVertex cpt1 , cpts1){
                foreach (MeshVertex cpt2 , cpts2){
                    if (cpt1.position == cpt2.position){
                        qDebug() << "exists jump " << cpt1.position;
                        rightMesh->addFace(cuttingContour_u[c_idx1].position, cuttingContour_u[c_idx2].position, cpt1.position);
                        rightMesh->addFace(cuttingContour_u[c_idx2].position, cuttingContour_u[c_idx1].position, cpt1.position);
                    }
                }
            }
        }
    }
    }

    if (!intersectionExists || cutFillMode==2){
    for (int c_idx1=0; c_idx1<cuttingContour_l.size(); c_idx1++){
        for (int c_idx2=0; c_idx2<cuttingContour_l.size(); c_idx2++){
            if (c_idx1 == c_idx2)
                continue;
            QList<MeshVertex> cpts1 = cuttingContourConnections_l.values(c_idx1);
            QList<MeshVertex> cpts2 = cuttingContourConnections_l.values(c_idx2);
            foreach(MeshVertex cpt1 , cpts1){
                foreach (MeshVertex cpt2 , cpts2){
                    if (cpt1.position == cpt2.position){
                        qDebug() << "exists jump " << cpt1.position;
                        rightMesh->addFace(cuttingContour_l[c_idx1].position, cuttingContour_l[c_idx2].position, cpt1.position);
                        rightMesh->addFace(cuttingContour_l[c_idx2].position, cuttingContour_l[c_idx1].position, cpt1.position);
                    }
                }
            }
        }
    }
    }

    if (cutFillMode == 2 && cuttingContour_u.size() >= 3){
        QVector3D normal_direction = QVector3D::normal(cuttingContour_u[0].position-cuttingContour_u[1].position, cuttingContour_u[2].position-cuttingContour_u[1].position);
        for (int u_idx=0; u_idx<cuttingContour_u.size(); u_idx++){
            if ((u_idx == 0 || u_idx == cuttingContour_u.size()-1) && intersectionExists)
                continue;
            MeshVertex umv1 = cuttingContour_u[u_idx];
            MeshVertex umv2 = cuttingContour_u[(u_idx+1)%cuttingContour_u.size()];

            MeshVertex lmv1 = cuttingContour_l[u_idx];
            MeshVertex lmv2 = cuttingContour_l[(u_idx+1)%cuttingContour_l.size()];

            if (normal_direction.z()>0){
                rightMesh->addFace(umv2.position, umv1.position, lmv1.position);
                rightMesh->addFace(umv2.position, lmv1.position, lmv2.position);
                leftMesh->addFace(umv1.position, umv2.position, lmv1.position);
                leftMesh->addFace(lmv1.position, umv2.position, lmv2.position);
            } else {
                rightMesh->addFace(umv1.position, umv2.position, lmv1.position);
                rightMesh->addFace(lmv1.position, umv2.position, lmv2.position);
                leftMesh->addFace(umv2.position, umv1.position, lmv1.position);
                leftMesh->addFace(umv2.position, lmv1.position, lmv2.position);
            }
        }

        if (intersectionExists){ // fill 대칭
            MeshVertex umv_enter = cuttingContour_u[0];
            MeshVertex umv1 = cuttingContour_u[1];
            MeshVertex lmv1 = cuttingContour_l[1];
            MeshVertex umv_exit = cuttingContour_u[cuttingContour_u.size()-1];
            MeshVertex umv_1 = cuttingContour_u[cuttingContour_u.size()-2];
            MeshVertex lmv_1 = cuttingContour_l[cuttingContour_l.size()-2];

            rightMesh->addFace(umv1.position, umv_enter.position, lmv1.position);
            rightMesh->addFace(umv_exit.position, umv_1.position, lmv_1.position);
            leftMesh->addFace(umv_enter.position, umv1.position, lmv1.position);
            leftMesh->addFace(umv_1.position, umv_exit.position, lmv_1.position);
            rightMesh->addFace(umv_enter.position, umv1.position, lmv1.position);
            rightMesh->addFace(umv_1.position, umv_exit.position, lmv_1.position);
            leftMesh->addFace(umv1.position, umv_enter.position, lmv1.position);
            leftMesh->addFace(umv_exit.position, umv_1.position, lmv_1.position);

            /*if (normal_direction.z()>0){
                qDebug() << "normal direction plus" << umv_enter.position << umv_exit.position;
                rightMesh->addFace(umv1.position, umv_enter.position, lmv1.position);
                rightMesh->addFace(umv_exit.position, umv_1.position, lmv_1.position);
                leftMesh->addFace(umv_enter.position, umv1.position, lmv1.position);
                leftMesh->addFace(umv_1.position, umv_exit.position, lmv_1.position);
            } else {
                qDebug() << "normal direction minus" << umv_enter.position << umv_exit.position;
                rightMesh->addFace(umv_enter.position, umv1.position, lmv1.position);
                rightMesh->addFace(umv_1.position, umv_exit.position, lmv_1.position);
                leftMesh->addFace(umv1.position, umv_enter.position, lmv1.position);
                leftMesh->addFace(umv_exit.position, umv_1.position, lmv_1.position);
            }*/
        }
    }

    if (leftMesh->faces.size() != 0)
        leftMesh->connectFaces();
    if (rightMesh->faces.size() != 0)
        rightMesh->connectFaces();
}

void modelcut::removeCuttingPoints(){

    for(int i=0;i<numPoints;i++)
    {
        sphereEntity[i]->removeComponent(sphereMesh[i]);
        sphereEntity[i]->removeComponent(sphereTransform[i]);
        sphereEntity[i]->removeComponent(sphereMaterial[i]);
    }
    /*delete [] sphereEntity;
    delete [] sphereMaterial;
    delete [] sphereMesh;
    delete [] sphereTransform;*/
    numPoints=0;
}

void modelcut::getSliderSignal(double value){
    QVector3D v1=cuttingPoints[cuttingPoints.size()-3];
    QVector3D v2=cuttingPoints[cuttingPoints.size()-2];
    QVector3D v3=cuttingPoints[cuttingPoints.size()-1];
    QVector3D world_origin(0,0,0);
    QVector3D original_normal(0,1,0);
    QVector3D desire_normal(QVector3D::normal(v1,v2,v3)); //size=1
    float angle = qAcos(QVector3D::dotProduct(original_normal,desire_normal))*180/M_PI+(value-1)*30;
    QVector3D crossproduct_vector(QVector3D::crossProduct(original_normal,desire_normal));
    for (int i=0;i<2;i++){
        planeTransform[i]->setRotation(QQuaternion::fromAxisAndAngle(crossproduct_vector, angle+180*i));
        planeTransform[i]->setTranslation(desire_normal*(-world_origin.distanceToPlane(v1,v2,v3)));
        planeEntity[i]->addComponent(planeTransform[i]);
    }
}
