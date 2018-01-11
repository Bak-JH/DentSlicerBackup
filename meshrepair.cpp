#include "meshrepair.h"

// removes unused vertices
void removeUnconnectedVertices(Mesh& mesh){

}

// removes totally unconnected faces
// if 2 edges are unconnected or 1 edge is unconnected, goto fillhole
void removeUnconnectedFaces(Mesh& mesh){
    /*for (int f_idx=0; f_idx<mesh.faces.size(); f_idx++){
        MeshFace &mf = mesh.faces[f_idx];
        vector<MeshFace>::iterator f_idx_it = mesh.faces.begin()+f_idx;
        int neighbor_cnt = 0;
        for (vector<vector<int>>::iterator it=mf.neighbor_faces.begin(); \
             it != mf.neighbor_faces.end(); \
             it ++){
            neighbor_cnt += (*it).size();
        }

        if (neighbor_cnt == 0){
            mesh.faces.erase(f_idx_it);
            return;
        }
    }

    for (int i=0; i<mesh.faces.size(); i++){
        MeshFace &mf = mesh.faces[i];
    }*/
}

// removes zero area triangles
void removeDegeneracy(Mesh& mesh){
    for (int f_idx=0; f_idx<mesh.faces.size(); f_idx++){
        MeshFace &mf = mesh.faces[f_idx];
        vector<MeshFace>::iterator f_idx_it = mesh.faces.begin()+f_idx;
        // one vertice case
        if (mf.mesh_vertex[0] == mf.mesh_vertex[1] && mf.mesh_vertex[1] == mf.mesh_vertex[2]){
            //remove face
            mesh.faces.erase(f_idx_it);
            // remove one vertice and change all faces that uses erased vertice.
            return;
        }

        // two vertices case
        if (mf.mesh_vertex[0] == mf.mesh_vertex[1]){

        }
        if (mf.mesh_vertex[1] == mf.mesh_vertex[2]){

        }
        if (mf.mesh_vertex[2] == mf.mesh_vertex[0]){

        }
    }
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


