#ifndef MESH_H
#define MESH_H
#include <vector>
#include <QVector>
#include <QVector3D>

class MeshVertex {
public:
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
    std::vector<MeshFace> faces;

    void addFace(QVector3D v0, QVector3D v1, QVector3D v2);
    void connectFaces();
    int findFaceWith2Vertices(int v0_idx, int v1_idx, int self_idx);
};

#endif // MESH_H
