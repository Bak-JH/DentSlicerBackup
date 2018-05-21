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

void cutAway(Mesh* leftMesh, Mesh* rightMesh, Mesh* mesh, vector<QVector3D> cuttingPoints){
    Path contour;
    Path3D cuttingContour;
    Paths3D cuttingEdges;
    int numPoints = cuttingPoints.size();

    for (int i=0; i<numPoints; i++){
        QVector3D cuttingPoint =cuttingPoints[i];
        contour.push_back(IntPoint(cuttingPoint.x(),cuttingPoint.y()));
        MeshVertex temp_mv;
        temp_mv.position = QVector3D(cuttingPoint.x(), cuttingPoint.y(), cuttingPoint.z());
        cuttingContour.push_back(temp_mv);
    }

    Paths3D innerContours, outerContours;

    foreach (MeshFace mf, mesh->faces){
        bool faceLeftToPlane = false;
        bool faceRightToPlane = false;

        Path3D leftContour;
        Path3D rightContour;

        Plane target_plane;
        for (int vn=0; vn<3; vn++){
            MeshVertex mv = mesh->vertices[mf.mesh_vertex[vn]];
            target_plane.push_back(mv.position);

            if (PointInPolygon(IntPoint(mv.position.x(), mv.position.y()), contour)){
                faceLeftToPlane = true;
                leftContour.push_back(mv);
            } else {
                faceRightToPlane = true;
                rightContour.push_back(mv);
            }
        }

        if (faceLeftToPlane && faceRightToPlane){ // cutting edge
            if (leftContour.size() == 2){
                innerContours.push_back(leftContour);
                /*for (MeshVertex mvc : leftContour)
                    innerContour.push_back(mvc);*/
            } else if (rightContour.size() == 2) {
                outerContours.push_back(rightContour);
                /*for (MeshVertex mvc : rightContour)
                    outerContour.push_back(mvc);*/
            }
        } else if (faceLeftToPlane){
            leftMesh->addFace(mesh->vertices[mf.mesh_vertex[0]].position, mesh->vertices[mf.mesh_vertex[1]].position, mesh->vertices[mf.mesh_vertex[2]].position);
        } else if (faceRightToPlane){
            rightMesh->addFace(mesh->vertices[mf.mesh_vertex[0]].position, mesh->vertices[mf.mesh_vertex[1]].position, mesh->vertices[mf.mesh_vertex[2]].position);
        }
    }
    Paths3D innerContours_c = contourConstruct3D(innerContours);
    Paths3D outerContours_c = contourConstruct3D(outerContours);

    qDebug() << "constructed inner outer contours : " << innerContours_c.size() << outerContours_c.size();
    for (Path3D innerContour : innerContours_c){ // connect inner contours
        qDebug() << innerContour.size();
        Path3D cuttingContour_c;

        for (MeshVertex imv : innerContour){
            float min_distance = 99999;
            MeshVertex min_distance_cmv;
            // find min distance cutting point and connect it
            for (MeshVertex cmv : cuttingContour){
                float cur_distance = cmv.position.distanceToPoint(imv.position);
                if (cur_distance < min_distance){
                    min_distance = cur_distance;
                    min_distance_cmv = cmv;
                }
            }
            min_distance_cmv.position = QVector3D(min_distance_cmv.position.x(), min_distance_cmv.position.y(), imv.position.z());
            cuttingContour_c.push_back(min_distance_cmv);
        }
        for (int imv_idx=0; imv_idx<innerContour.size(); imv_idx ++){
            MeshVertex in1 = innerContour[imv_idx];
            MeshVertex in2 = innerContour[imv_idx%innerContour.size()];
            MeshVertex cut1 = cuttingContour_c[imv_idx];
            MeshVertex cut2 = cuttingContour_c[imv_idx%cuttingContour_c.size()];

            //qDebug() << "adding face " << in1.position << in2.position << cut1.position;
            leftMesh->addFace(in1.position, in2.position, cut1.position);
            leftMesh->addFace(cut1.position, in2.position, cut2.position);
        }
        //interpolate(leftMesh, innerContour, cuttingContour_c);
    }
    for (Path3D outerContour : outerContours_c){
        qDebug() << outerContour.size();
        Path3D cuttingContour_c;

        for (MeshVertex omv : outerContour){
            float min_distance = 99999;
            MeshVertex min_distance_cmv;
            // find min distance cutting point and connect it
            for (MeshVertex cmv : cuttingContour){
                float cur_distance = cmv.position.distanceToPoint(omv.position);
                if (cur_distance < min_distance){
                    min_distance = cur_distance;
                    min_distance_cmv = cmv;
                }
            }
            min_distance_cmv.position = QVector3D(min_distance_cmv.position.x(), min_distance_cmv.position.y(), omv.position.z());
            cuttingContour_c.push_back(min_distance_cmv);
        }
        for (int omv_idx=0; omv_idx<outerContour.size(); omv_idx ++){
            MeshVertex on1 = outerContour[omv_idx];
            MeshVertex on2 = outerContour[omv_idx%outerContour.size()];
            MeshVertex cut1 = cuttingContour_c[omv_idx];
            MeshVertex cut2 = cuttingContour_c[omv_idx%cuttingContour_c.size()];

            rightMesh->addFace(on1.position, on2.position, cut1.position);
            rightMesh->addFace(cut1.position, on2.position, cut2.position);
        }
        //interpolate(rightMesh, outerContour, cuttingContour_c);
    }
    leftMesh->connectFaces();
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
