#include "meshrepair.h"
#include "modelcut.h"
#include "qmlmanager.h"

void MeshRepair::repairMesh(Mesh* mesh){

    qDebug() << "mesh repair start from mesh size :" << mesh->getFaces()->size();
    /*// remove Unconnected
    //removeUnconnected(mesh);
    qDebug() << "removed unconnected";
    qmlManager->setProgress(0.1);

    // remove degenerate
    //removeDegenerate(mesh);
    qDebug() << "removed degenerate";
    qmlManager->setProgress(0.25);*/

    // find holes
    auto holes = identifyHoles(mesh);
    qmlManager->setProgress(0.3);

    // fill holes
    fillHoles(mesh, holes);
    qDebug() << "filled holes";
    qmlManager->setProgress(0.5);

    // fix normal orientations
    fixNormalOrientations(mesh);
    qDebug() << "fixed normal orientations";
    qmlManager->setProgress(0.8);

    // remove gaps
    //removeGaps(mesh);
    qDebug() << "removed gaps";
    qmlManager->setProgress(1);
}


// no need to remove unused vertices since it iterates faces
// removes totally unconnected faces
// if 2 edges are unconnected or 1 edge is unconnected, goto fillhole
void MeshRepair::removeUnconnected(Mesh* mesh){
    int unconnected_cnt = 0;
    int face_idx = 0;

    std::vector<MeshFace>::const_iterator mf_it = mesh->getFaces()->cbegin();

    while (mf_it != mesh->getFaces()->cend()){
        face_idx ++;
        if (face_idx %100 ==0)
            QCoreApplication::processEvents();
        const MeshFace& mf = (*mf_it);
        int neighbor_cnt = 0;

        for (int i=0; i<3; i++){
            neighbor_cnt += mf.neighboring_faces[i].size();
        }

        if (neighbor_cnt == 0){
            qDebug() << "unconnected face";
            mf_it = mesh->removeFace(mf_it);
            unconnected_cnt ++;
        } else {
            mf_it ++;
        }
    }
    qDebug() << unconnected_cnt << "unconnected faces found, faces cnt :" << mesh->faces.size();
}

// removes zero area triangles
void MeshRepair::removeDegenerate(Mesh* mesh){
    int degenerate_cnt = 0;
    int face_idx = 0;

    std::vector<MeshFace>::iterator mf_it = mesh->faces.begin();
    while (mf_it != mesh->faces.end()){
        face_idx ++;
        if (face_idx %100 ==0)
            QCoreApplication::processEvents();
        MeshFace &mf = (*mf_it);

        // one vertice && 2 vertices case
        if (mf.mesh_vertex[0] == mf.mesh_vertex[1] || mf.mesh_vertex[1] == mf.mesh_vertex[2] || mf.mesh_vertex[2] == mf.mesh_vertex[0]){
            //remove face
            mf_it = mesh->removeFace(mf_it);
            degenerate_cnt ++;
            // remove one vertice and change all faces that uses erased vertice.
        } else {
            mf_it ++;
        }
    }
    qDebug() << degenerate_cnt << "degenerate faces found, faces cnt :" << mesh->faces.size();
}

Paths3D MeshRepair::identifyHoles(const Mesh* mesh){

    // used for auto repair steps
    Paths3D holes;
    int face_idx = 0;

    for (const MeshFace &mf : (*mesh->getFaces())){
        face_idx ++;
        if (face_idx %100 ==0)
            QCoreApplication::processEvents();

        //qDebug() << "neighbors " << mf.neighboring_faces[0].size() << mf.neighboring_faces[1].size() << mf.neighboring_faces[2].size();
        if (mf.neighboring_faces[0].size() == 0){ // edge 0 is unconnected
            Path3D temp_edge;
            temp_edge.push_back(mesh->idx2MV(mf.mesh_vertex[0]));
            temp_edge.push_back(mesh->idx2MV(mf.mesh_vertex[1]));
            holes.push_back(temp_edge);
        }
        if (mf.neighboring_faces[1].size() == 0){ // edge 1 is unconnected
            Path3D temp_edge;
            temp_edge.push_back(mesh->idx2MV(mf.mesh_vertex[1]));
            temp_edge.push_back(mesh->idx2MV(mf.mesh_vertex[2]));
            holes.push_back(temp_edge);
        }
        if (mf.neighboring_faces[2].size() == 0){ // edge 2 is unconnected
            Path3D temp_edge;
            temp_edge.push_back(mesh->idx2MV(mf.mesh_vertex[2]));
            temp_edge.push_back(mesh->idx2MV(mf.mesh_vertex[0]));
            holes.push_back(temp_edge);
        }
    }
    qDebug()<< "hole edges coutn : " << holes.size();

    // get closed contour from hole_edges
    holes = contourConstruct3D(holes);
    qDebug() << "hole detected";

    /*for (Paths3D::iterator ps_it = mesh->holes.begin(); ps_it != mesh->holes.end();){

        if (ps_it->size() <3)
            ps_it = mesh->holes.erase(ps_it);
        ps_it++;
    }*/
    qDebug() << "mesh hole count :" << holes.size();
    return holes;
}


// detects hole and remove them
void MeshRepair::fillHoles(Mesh* mesh, const Paths3D& holes){
    for (Path3D hole : holes){
        qDebug() << "filling hole" << hole.size() << "sized";
        if (hole.size() <= 2) { // if edge is less than 3 (no hole)
            continue;
        }

        QVector3D centerOfMass = QVector3D(0,0,0);
        for (MeshVertex mv : hole){
            centerOfMass += mv.position;
        }
        centerOfMass /= hole.size();

        // get orientation
        /*bool ccw = true;
        QVector3D current_plane_normal = QVector3D::normal(hole[1].position, centerOfMass, hole[0].position);
        if (QVector3D::dotProduct(current_plane_normal, plane_normal)>0){
            ccw = false;
        }*/

        for (int i=0; i<hole.size(); i++){
            mesh->addFace(hole[i].position, centerOfMass, hole[(i+1)%hole.size()].position);
            mesh->addFace(hole[(i+1)%hole.size()].position, centerOfMass, hole[i].position);
        }
        /*mesh->addFace(QVector3D(0,0,0), QVector3D(100,100,0), QVector3D(100,0,0));
        mesh->addFace(QVector3D(0,0,0), QVector3D(1000,100,0), QVector3D(100,0,0));*/
/*
        for (int i=0; i<hole.size(); i++){
            if (ccw){
                mesh->addFace(hole[i].position, centerOfMass, hole[(i+1)%hole.size()].position);
            } else {
                mesh->addFace(hole[(i+1)%hole.size()].position, centerOfMass, hole[i].position);
            }
        }
        */

        /*for (int i=0; i<contour.size(); i++){
            if (ccw){
                leftMesh->addFace(contour[i].position, centerOfMass, contour[(i+1)%contour.size()].position);
                rightMesh->addFace(contour[(i+1)%contour.size()].position, centerOfMass, contour[i].position);
            } else {
                leftMesh->addFace(contour[(i+1)%contour.size()].position, centerOfMass, contour[i].position);
                rightMesh->addFace(contour[i].position, centerOfMass, contour[(i+1)%contour.size()].position);
            }
        }*/

        /*
        qDebug() << "ok till here1";
        // fill holes
        qDebug() << "ok till here2";
        for (std::array<QVector3D,3> face : fillPath(hole)){
            mesh->addFace(face[0],face[1],face[2]);
        }
        qDebug() << "ok till here3";

        */

        /*Path3D prev_path;
        Path3D cur_path;

        for (Path3D::iterator vert_it = hole.begin()+1; vert_it != hole.end()-1; ++vert_it){
            MeshVertex prev_vert = (*(vert_it-1));
            MeshVertex cur_vert = (*vert_it);
            MeshVertex next_vert = (*(vert_it+1));

            prev_path.clear();
            prev_path.push_back(prev_vert);
            prev_path.push_back(cur_vert);
            cur_path.clear();
            cur_path.push_back(cur_vert);
            cur_path.push_back(next_vert);
            reverse(cur_path.begin(), cur_path.end());

            int div_cnt = suggestDivisionCnt(prev_path, cur_path);
            for ( ; div_cnt>=0 ; div_cnt--){
                // do something abount division
            }
        }*/
    }
    mesh->connectFaces();

    qDebug() << "ok till here4";
}

std::vector<std::array<QVector3D, 3>> MeshRepair::fillPath(Path3D path){
    std::vector<std::array<QVector3D, 3>> result;
    result.reserve(path.size()*10);

    if (path.size() <3)
        return result;

    Plane maximal_plane;
    maximal_plane.push_back(path[0].position);
    maximal_plane.push_back(path[1].position);
    maximal_plane.push_back(path[2].position);

    qDebug() << "fill path debug 1";
    for (MeshVertex mv : path){
        if (modelcut::isLeftToPlane(maximal_plane, mv.position)){
            // move plane to normal direction with distance from mv
            QVector3D plane_normal = QVector3D::normal(maximal_plane[0],maximal_plane[1],maximal_plane[2]);
            float distance = mv.position.distanceToPlane(maximal_plane[0],maximal_plane[1],maximal_plane[2]);
            for (int i=0; i<3; i++){
                maximal_plane[i] += distance*plane_normal;
            }
        }
    }
    qDebug() << "fill path debug 2";

    // project path to target path
    Path3D target_path;
    target_path.reserve(path.size());

    QVector3D plane_normal = QVector3D::normal(maximal_plane[0],maximal_plane[1],maximal_plane[2]);

    qDebug() << "fill path debug 3";
    for (int i=0; i<path.size(); i++){
        target_path.push_back(path[i]);

        // project qvector3d to maximal plane
        float distance = path[i].position.distanceToPlane(maximal_plane[0],maximal_plane[1],maximal_plane[2]);
        target_path.end()->position += plane_normal * distance;
    }
    qDebug() << "fill path debug 4";

    //std::vector<QVector3D> interpolated = interpolate(path, target_path);
    //qDebug() << "interpolated size : " << interpolated.size();
    // interpolate between path and target path
    for (std::array<QVector3D, 3> face : interpolate(path, target_path)){
        result.push_back(face);
    }

    qDebug() << "fill path debug 5";

    // fill Hole
    int half_path_size = path.size()/2;

    // fill hole really
    for (int i=0; i<half_path_size-1; i++){
        std::array<QVector3D,3> temp_face;
        temp_face[0] = path[i].position;
        temp_face[1] = path[path.size()-i-2].position;
        temp_face[2] = path[i+1].position;
        result.push_back(temp_face);
        temp_face[0] = path[i+1].position;
        temp_face[1] = path[path.size()-i-2].position;
        temp_face[2] = path[path.size()-i-3].position;
        result.push_back(temp_face);
    }
    qDebug() << "fill path debug 6";

    if (path.size()%2 != 0){
        std::array<QVector3D,3> temp_face;
        temp_face[0] = path[0].position;
        temp_face[1] = path[path.size()-1].position;
        temp_face[2] = path[path.size()-2].position;
        result.push_back(temp_face);
    }
    qDebug() << "fill path debug 7";

    return result;
}


int MeshRepair::suggestDivisionCnt(Path3D e1,Path3D e2){
    QVector3D e1_diff = e1[1].position-e1[0].position;
    QVector3D e2_diff = e2[1].position-e2[0].position;

    float norm_prod = e1_diff.length() * e2_diff.length();
    float dot_prod = QVector3D::dotProduct(e1_diff, e2_diff);

    float cos = dot_prod/norm_prod;


    if (cos >= cos50){
        return 0;
    } else if (cos100<=cos<cos50){
        return 1;
    } else if (cos150<=cos<cos100){
        return 2;
    } else {
        return 3;
    }
    return 4;
}

// detects orientation defects, which induces normal std::vector errors and render errors
void MeshRepair::fixNormalOrientations(Mesh* mesh){

}

// future works
void MeshRepair::removeGaps(Mesh* mesh){

}


/*---------------- helper functions -----------------*/


