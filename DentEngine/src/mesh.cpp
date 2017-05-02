#include "mesh.h"
#include <QDebug>

Mesh::Mesh(){

}

void Mesh::addFace(QVector3D v0, QVector3D v1, QVector3D v2){
    MeshVertex mv0 = MeshVertex(v0);
    MeshVertex mv1 = MeshVertex(v1);
    MeshVertex mv2 = MeshVertex(v2);
    int vertex_idx = vertices.size();
    mv0.idx = vertex_idx;
    mv1.idx = vertex_idx + 1;
    mv2.idx = vertex_idx + 2;
    vertices.push_back(mv0);
    vertices.push_back(mv1);
    vertices.push_back(mv2);

    MeshFace mf;
    mf.idx = faces.size();
    mf.mesh_vertex[0] = mv0.idx;
    mf.mesh_vertex[1] = mv1.idx;
    mf.mesh_vertex[2] = mv2.idx;

    vertices[mv0.idx].connected_faces.push_back(mf.idx);
    vertices[mv1.idx].connected_faces.push_back(mf.idx);
    vertices[mv2.idx].connected_faces.push_back(mf.idx);
    faces.push_back(mf);
}

// add connected face idx to each meshes
void Mesh::connectFaces(){
    for (int i=0; i<faces.size(); i++){
        MeshFace mf = faces[i];
        mf.connected_face_idx[0] = findFaceWith2Vertices(mf.mesh_vertex[0], mf.mesh_vertex[1], i); // connected to vertex 0 1
    }
}

// find face containing 2 vertices presented as arguments
int Mesh::findFaceWith2Vertices(int v0_idx, int v1_idx, int self_idx){
    std::vector<int> candidates;
    for (int f: vertices[v0_idx].connected_faces){
        if (f == self_idx)
            continue;
        if (faces[f].mesh_vertex[0] == v1_idx || faces[f].mesh_vertex[1] == v1_idx || faces[f].mesh_vertex[2] == v1_idx){
            candidates.push_back(f);
        }
    }
    if (candidates.size() == 1){
        return candidates[0];
    } else {
        qDebug() << "multiple faces are connected so outputting first connected face";
        return candidates[0];
    }
}
