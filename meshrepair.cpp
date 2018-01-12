#include "meshrepair.h"

void repairMesh(Mesh& mesh){

    qDebug() << "mesh repair start from mesh size :" << mesh.faces.size();
    // remove Unconnected
    removeUnconnected(mesh);
    qDebug() << "removed unconnected";

    // remove degenerate
    removeDegenerate(mesh);
    qDebug() << "removed degenerate";

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
void removeUnconnected(Mesh& mesh){
    int unconnected_cnt = 0;

    for (vector<MeshFace>::iterator mf_it = mesh.faces.begin(); mf_it != mesh.faces.end() ;){
    //for (int f_idx=0; f_idx<mesh.faces.size(); f_idx++){
        //MeshFace &mf = mesh.faces[f_idx];
        MeshFace &mf = (*mf_it);
        int neighbor_cnt = 0;
        for (vector<vector<MeshFace>>::iterator it=mf.neighboring_faces.begin(); \
             it != mf.neighboring_faces.end(); \
             ++it){
            neighbor_cnt += (*it).size();
        }

        if (neighbor_cnt == 0){
            qDebug() << "unconnected face";
            mf_it = mesh.removeFace(mf_it);
            unconnected_cnt ++;
        } else {
            mf_it ++;
        }
    }
    qDebug() << unconnected_cnt << "unconnected points found, faces cnt :" << mesh.faces.size();
}

// removes zero area triangles
void removeDegenerate(Mesh& mesh){
    int degenerate_cnt = 0;

    for (vector<MeshFace>::iterator mf_it = mesh.faces.begin(); mf_it != mesh.faces.end() ;){
    //for (int f_idx=0; f_idx<mesh.faces.size(); f_idx++){
        //MeshFace &mf = mesh.faces[f_idx];
        MeshFace &mf = (*mf_it);
        // one vertice && 2 vertices case
        if (mf.mesh_vertex[0] == mf.mesh_vertex[1] || mf.mesh_vertex[1] == mf.mesh_vertex[2] || mf.mesh_vertex[2] == mf.mesh_vertex[0]){
            //remove face
            //qDebug() << "degenerate face";
            mf_it = mesh.removeFace(mf_it);
            degenerate_cnt ++;
            //mesh.removeFace(f_idx);
            //f_idx --;
            // remove one vertice and change all faces that uses erased vertice.
        } else {
            mf_it ++;
        }
    }
    qDebug() << degenerate_cnt << "degenerate points found, faces cnt :" << mesh.faces.size();
}

// detects hole and remove them
void fillHoles(Mesh& mesh){

}

// detects orientation defects, which induces normal vector errors and render errors
void fixNormalOrientations(Mesh& mesh){

}

// future works
void removeGaps(Mesh& mesh){

}


