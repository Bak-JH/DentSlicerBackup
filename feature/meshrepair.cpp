#include "meshrepair.h"
#include "modelcut.h"

void repairMesh(Mesh* mesh){

    qDebug() << "mesh repair start from mesh size :" << mesh->faces.size();
    // remove Unconnected
    removeUnconnected(mesh);
    qDebug() << "removed unconnected";

    // remove degenerate
    removeDegenerate(mesh);
    qDebug() << "removed degenerate";

    // find holes
    identifyHoles(mesh);

    // fill holes
    fillHoles(mesh);
    qDebug() << "filled holes";

    // fix normal orientations
    fixNormalOrientations(mesh);
    qDebug() << "fixed normal orientations";

    // remove gaps
    removeGaps(mesh);
    qDebug() << "removed gaps";
}


// no need to remove unused vertices since it iterates faces
// removes totally unconnected faces
// if 2 edges are unconnected or 1 edge is unconnected, goto fillhole
void removeUnconnected(Mesh* mesh){
    int unconnected_cnt = 0;
    int face_idx = 0;

    vector<MeshFace>::iterator mf_it = mesh->faces.begin();

    while (mf_it != mesh->faces.end()){
        face_idx ++;
        if (face_idx %100 ==0)
            QCoreApplication::processEvents();
        MeshFace& mf = (*mf_it);
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
void removeDegenerate(Mesh* mesh){
    int degenerate_cnt = 0;
    int face_idx = 0;

    vector<MeshFace>::iterator mf_it = mesh->faces.begin();
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

void identifyHoles(Mesh* mesh){
    int face_idx = 0;
    Paths3D hole_edges;

    for (MeshFace &mf : mesh->faces){
        face_idx ++;
        if (face_idx %100 ==0)
            QCoreApplication::processEvents();
        if (mf.neighboring_faces[0].size() == 0){ // edge 0 is unconnected
            Path3D temp_edge;
            temp_edge.push_back(mesh->idx2MV(mf.mesh_vertex[0]));
            temp_edge.push_back(mesh->idx2MV(mf.mesh_vertex[1]));
            hole_edges.push_back(temp_edge);
        }
        if (mf.neighboring_faces[1].size() == 0){ // edge 1 is unconnected
            Path3D temp_edge;
            temp_edge.push_back(mesh->idx2MV(mf.mesh_vertex[1]));
            temp_edge.push_back(mesh->idx2MV(mf.mesh_vertex[2]));
            hole_edges.push_back(temp_edge);
        }
        if (mf.neighboring_faces[2].size() == 0){ // edge 2 is unconnected
            Path3D temp_edge;
            temp_edge.push_back(mesh->idx2MV(mf.mesh_vertex[2]));
            temp_edge.push_back(mesh->idx2MV(mf.mesh_vertex[0]));
            hole_edges.push_back(temp_edge);
        }
    }
    qDebug()<< "hole edges coutn : " << hole_edges.size();

    // get closed contour from hole_edges
    mesh->holes = contourConstruct(hole_edges);
    qDebug() << "mesh hole count :" << mesh->holes.size();
    return;
}


// detects hole and remove them
void fillHoles(Mesh* mesh){
    for (Path3D hole : mesh->holes){
        if (hole.size() <= 2) { // if edge is less than 3 (no hole)
            continue;
        }

        // fill holes
        vector<vector<QVector3D>> cap = fillPath(hole);
        for (vector<QVector3D> face : cap){
            mesh->addFace(face[0],face[1],face[2]);
        }

        mesh->connectFaces();

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
}

vector<vector<QVector3D>> fillPath(Path3D path){
    vector<vector<QVector3D>> result;
    if (path.size() <3)
        return result;

    Plane maximal_plane;
    maximal_plane.push_back(path[0].position);
    maximal_plane.push_back(path[1].position);
    maximal_plane.push_back(path[2].position);

    // update maximal plane
    while(path.size() != 3){
        for (vector<MeshVertex>::iterator p_it =path.begin(); p_it != path.end(); ++p_it){
            if (!isLeftToPlane(maximal_plane, p_it->position)){
                maximal_plane.pop_back();
                path.erase(p_it);
                maximal_plane.push_back(p_it->position);
                break;
            }
        }
    }

    // project path to target path
    Path3D target_path;
    target_path.reserve(path.size());

    QVector3D plane_normal = QVector3D::normal(maximal_plane[0],maximal_plane[1],maximal_plane[2]);

    for (int i=0; i<path.size(); i++){
        target_path.push_back(path[i]);

        // project qvector3d to maximal plane
        float distance = path[i].position.distanceToPlane(maximal_plane[0],maximal_plane[1],maximal_plane[2]);
        target_path.end()->position += plane_normal * distance;
    }

    // interpolate between path and target path
    result = interpolate(path, target_path);

    // fill Hole
    int half_path_size = path.size()/2;

    for (int i=0; i<half_path_size-1; i++){
        vector<QVector3D> temp_face;
        temp_face.push_back(path[i].position);
        temp_face.push_back(path[path.size()-i-2].position);
        temp_face.push_back(path[i+1].position);
        result.push_back(temp_face);
        temp_face.clear();
        temp_face.push_back(path[i+1].position);
        temp_face.push_back(path[path.size()-i-2].position);
        temp_face.push_back(path[path.size()-i-3].position);
        result.push_back(temp_face);
    }

    if (path.size()%2 != 0){
        vector<QVector3D> temp_face;
        temp_face.push_back(path[0].position);
        temp_face.push_back(path[path.size()-1].position);
        temp_face.push_back(path[path.size()-2].position);
        result.push_back(temp_face);
    }

    return result;
}


int suggestDivisionCnt(Path3D e1,Path3D e2){
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

// detects orientation defects, which induces normal vector errors and render errors
void fixNormalOrientations(Mesh* mesh){

}

// future works
void removeGaps(Mesh* mesh){

}


/*---------------- helper functions -----------------*/


