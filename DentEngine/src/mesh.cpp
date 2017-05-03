#include "mesh.h"
#include <QDebug>

Mesh::Mesh(){

};

void Mesh::addFace(QVector3D v0, QVector3D v1, QVector3D v2){
    int v0_idx = getVertexIdx(v0);
    int v1_idx = getVertexIdx(v1);
    int v2_idx = getVertexIdx(v2);

    qDebug() << v0_idx << v1_idx << v2_idx;

    MeshFace mf;
    mf.idx = faces.size();
    mf.mesh_vertex[0] = v0_idx;
    mf.mesh_vertex[1] = v1_idx;
    mf.mesh_vertex[2] = v2_idx;

    vertices[v0_idx].connected_faces.emplace_back(mf.idx);
    vertices[v1_idx].connected_faces.emplace_back(mf.idx);
    vertices[v2_idx].connected_faces.emplace_back(mf.idx);

    faces.emplace_back(mf);
}

// add connected face idx to each meshes
void Mesh::connectFaces(){
    for (int i=0; i<faces.size(); i++){
        MeshFace mf = faces[i];
        mf.connected_face_idx[0] = findFaceWith2Vertices(mf.mesh_vertex[0], mf.mesh_vertex[1], i); // connected to vertex 0 1
        mf.connected_face_idx[1] = findFaceWith2Vertices(mf.mesh_vertex[1], mf.mesh_vertex[2], i); // connected to vertex 0 1
        mf.connected_face_idx[2] = findFaceWith2Vertices(mf.mesh_vertex[2], mf.mesh_vertex[0], i); // connected to vertex 0 1
    }
}

/********************** Helper Functions **********************/

int64_t vertexHash(QVector3D v) // max build size = 1000mm, resolution = 1 micron
{
    return ((int64_t)(v.x() / vertex_meld_distance)) ^\
            (((int64_t)(v.y() / vertex_meld_distance)) << 21) ^\
            (((int64_t)(v.z() / vertex_meld_distance)) << 42);
}

int Mesh::getVertexIdx(QVector3D v){
    int vertex_idx = -1;
    int64_t vertex_hash = vertexHash(v);

    if (vertices_hash.contains(vertex_hash)){ // if vertex exists
        vertex_idx = vertices_hash.value(vertex_hash).idx; // if multiple, hash collision
    } else { // if vertex doesn't exists
        MeshVertex mv = MeshVertex(v);
        mv.idx = vertices.size();
        vertices.emplace_back(mv);
        vertices_hash.insert(vertex_hash, mv);
        vertex_idx = mv.idx;
    }
    return vertex_idx;
}

// find face containing 2 vertices presented as arguments
int Mesh::findFaceWith2Vertices(int v0_idx, int v1_idx, int self_idx){
    std::vector<int> candidates;
    for (int f: vertices[v0_idx].connected_faces){
        if (f == self_idx)
            continue;
        if (faces[f].mesh_vertex[0] == v1_idx || faces[f].mesh_vertex[1] == v1_idx || faces[f].mesh_vertex[2] == v1_idx){
            candidates.emplace_back(f);
        }
    }
    if (candidates.size() == 0){
        //qDebug() << "no candidates";
        return -1;
    }
    if (candidates.size() == 1){
        //qDebug() << "found face";
        return candidates[0];
    } else {
        qDebug() << candidates.size() << "multiple faces are connected so outputting first connected face" << candidates[0];
        return candidates[0];
    }
}
