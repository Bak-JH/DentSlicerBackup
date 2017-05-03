#ifndef MESH_H
#define MESH_H
#include <vector>
#include <QVector3D>
#include <QHash>

const float vertex_meld_distance = 0.001; // resolution in mm (0.0001 and 0.0009 are same, 1 micron)

class MeshVertex {
public:
    MeshVertex(){}
    int idx;
    QVector3D position;
    std::vector<int> connected_faces;
    MeshVertex(QVector3D position): position(position) {connected_faces.reserve(8);}
};

class MeshFace {
public:
    int idx;
    int mesh_vertex[3] = {-1};
    int connected_face_idx[3];
};

class Mesh{
public :
    Mesh();
    std::vector<MeshVertex> vertices;
    QHash<int64_t, MeshVertex> vertices_hash;
    std::vector<MeshFace> faces;

    void addFace(QVector3D v0, QVector3D v1, QVector3D v2);
    int getVertexIdx(QVector3D v);
    void connectFaces();
    int findFaceWith2Vertices(int v0_idx, int v1_idx, int self_idx);
};

int64_t vertexHash(QVector3D v);

#endif // MESH_H
