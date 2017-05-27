#ifndef MESH_H
#define MESH_H
#include <vector>
#include <QVector3D>
#include <QHash>
#include "configuration.h"
#include "polyclipping/clipper.hpp"

using namespace std;
using namespace ClipperLib;

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
    Mesh(Configuration* cfg) : cfg(cfg) {}
    Configuration* cfg;
    std::vector<MeshVertex> vertices;
    QHash<int64_t, MeshVertex> vertices_hash;
    std::vector<MeshFace> faces;
    float x_min, x_max, y_min, y_max, z_min, z_max;

    /********************** Mesh Generation Functions **********************/
    void addFace(QVector3D v0, QVector3D v1, QVector3D v2);
    void connectFaces();

    /********************** Path Generation Functions **********************/
    void addPoint(float x, float y, Path *path);
    Path intersectionPath(MeshFace mf, float z);

    /********************** Helper Functions **********************/
    int getVertexIdx(QVector3D v);
    void updateMinMax(QVector3D v);
    int findFaceWith2Vertices(int v0_idx, int v1_idx, int self_idx);
    float getFaceZmin(MeshFace mf);
    float getFaceZmax(MeshFace mf);
    MeshFace idx2MF(int idx);
    MeshVertex idx2MV(int idx);
};

int64_t vertexHash(QVector3D v);


#endif // MESH_H
