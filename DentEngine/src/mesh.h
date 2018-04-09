#ifndef MESH_H
#define MESH_H
#include <vector>
#include <Qt3DCore/qtransform.h>
#include <QVector3D>
#include <QHash>
#include "configuration.h"
#include "polyclipping/clipper/clipper.hpp"

#define cos50 0.64278761
#define cos100 -0.17364818
#define cos150 -0.8660254


using namespace std;
using namespace ClipperLib;

// plane contains at least 3 vertices contained in the plane in clockwise direction
typedef vector<QVector3D> Plane;

class MeshFace {
public:
    MeshFace() {}
    //MeshFace() {neighboring_faces.reserve(10);}

    int idx;
    int mesh_vertex[3] = {-1,-1,-1};
    //int connected_face_idx[3];

    vector<vector<MeshFace*>> neighboring_faces;

    QVector3D fn;
    QVector3D fn_unnorm;
};

class MeshVertex {
public:
    MeshVertex(){}
    int idx;
    QVector3D position;
    QVector3D vn;
    std::vector<MeshFace*> connected_faces;
    MeshVertex(QVector3D position): position(position) {}//connected_faces.reserve(8);}

    friend inline bool operator== (const MeshVertex& a, const MeshVertex& b){
        return a.position == b.position;
    }

    friend inline bool operator!= (const MeshVertex& a, const MeshVertex& b){
        return a.position != b.position;
    }
};

typedef vector<MeshVertex> Path3D;
typedef vector<Path3D> Paths3D;

class Mesh{
public :
    Mesh() {};
    std::vector<MeshVertex> vertices;
    QHash<uint32_t, MeshVertex> vertices_hash;
    std::vector<MeshFace> faces;

    // used for auto repair steps
    Paths3D holes;

    float x_min = 99999, x_max = 99999, y_min = 99999, y_max = 99999, z_min = 99999, z_max = 99999;

    /********************** Mesh Edit Functions***********************/
    void vertexOffset(float factor);
    void vertexMove(QVector3D direction);
    void vertexRotate(QMatrix4x4 tmpmatrix);
    void vertexScale(float scale);

    /********************** Mesh Generation Functions **********************/
    void addFace(QVector3D v0, QVector3D v1, QVector3D v2);
    vector<MeshFace>::iterator removeFace(vector<MeshFace>::iterator f_it);
    void connectFaces();

    /********************** Path Generation Functions **********************/
    void addPoint(float x, float y, Path *path);
    Path intersectionPath(MeshFace mf, float z);

    /********************** Helper Functions **********************/
    int getVertexIdx(QVector3D v);
    void updateMinMax(QVector3D v);
    vector<MeshFace*> findFaceWith2Vertices(int v0_idx, int v1_idx, MeshFace self_f);
    float getFaceZmin(MeshFace mf);
    float getFaceZmax(MeshFace mf);
    MeshFace idx2MF(int idx);
    MeshVertex idx2MV(int idx);
};

uint32_t vertexHash(QVector3D v);
float vertexDistance(QVector3D, QVector3D);


#endif // MESH_H
