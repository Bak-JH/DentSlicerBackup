#ifndef MESH_H
#define MESH_H
#include <vector>
#include <Qt3DCore/qtransform.h>
#include <QVector3D>
#include <QHash>
#include "configuration.h"
#include "polyclipping/clipper/clipper.hpp"
#include "polyclipping/poly2tri/poly2tri.h"
#include <QTransform>
#include <QTime>

#define cos50 0.64278761
#define cos100 -0.17364818
#define cos150 -0.8660254
#define FZERO 0.00001f

using namespace std;
using namespace ClipperLib;

// plane contains at least 3 vertices contained in the plane in clockwise direction
typedef vector<QVector3D> Plane;

class MeshFace {
public:
    MeshFace() {}

    int idx;
    int parent_idx = -1;
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

//typedef vector<MeshVertex> Path3D;
class Path3D : public vector<MeshVertex>{
    public:
        Path projection;
        vector<Path3D> inner;
        vector<Path3D> outer;
};

typedef vector<Path3D> Paths3D;

class Mesh{
public :
    Mesh() {};
    std::vector<MeshVertex> vertices;
    QHash<uint32_t, MeshVertex> vertices_hash;
    std::vector<MeshFace> faces;

    // for undo & redo
    Mesh* prevMesh = nullptr;
    Mesh* nextMesh = nullptr;

    QTime time;
    QVector3D m_translation;
    QMatrix4x4 m_matrix;

    // used for auto repair steps
    Paths3D holes;

    // used for freecut, autoarrange elsewhere
    Path convexHull;

    float x_min = 99999, x_max = 99999, y_min = 99999, y_max = 99999, z_min = 99999, z_max = 99999;

    /********************** Mesh Edit Functions***********************/
    void vertexOffset(float factor);
    void vertexMove(QVector3D direction);
    Mesh* vertexMoved(QVector3D direction);
    void centerMesh();
    void vertexRotate(QMatrix4x4 tmpmatrix);
    void vertexScale(float scaleX, float scaleY, float scaleZ, float centerX, float centerY);
    Mesh* copyMesh();
    void reverseFaces();

    /********************** Mesh Generation Functions **********************/
    void addFace(QVector3D v0, QVector3D v1, QVector3D v2);
    void addFace(QVector3D v0, QVector3D v1, QVector3D v2, int parent_idx);
    vector<MeshFace>::iterator removeFace(vector<MeshFace>::iterator f_it);
    void removeFace(MeshFace* mf);
    void connectFaces();

    /********************** Path Generation Functions **********************/
    void addPoint(float x, float y, Path *path);
    Paths3D intersectionPaths(Path Contour, Plane target_plane);
    Path3D intersectionPath(Plane base_plane, Plane target_plane);
    Path intersectionPath(MeshFace mf, float z);

    /********************** Helper Functions **********************/
    int getVertexIdx(QVector3D v);
    float* calculateMinMax(QMatrix4x4 rotmatrix);
    void updateMinMax(QVector3D v);
    vector<MeshFace*> findFaceWith2Vertices(int v0_idx, int v1_idx, MeshFace self_f);
    float getFaceZmin(MeshFace mf);
    float getFaceZmax(MeshFace mf);
    MeshFace idx2MF(int idx);
    MeshVertex idx2MV(int idx);
};


float round(float num, int precision);
bool doubleAreSame(double a, double b);

uint32_t vertexHash(QVector3D v);
float vertexDistance(QVector3D, QVector3D);

QHash<uint32_t, Path>::iterator findSmallestPathHash(QHash<uint32_t, Path> pathHash);

// construct closed contour using segments created from identify step
Paths contourConstruct(Paths);
Paths3D contourConstruct3D(Paths3D hole_edges);

/* class containmentPath{
public:
    Path projection;
    vector<containmentPath> inner;
    vector<containmentPath> outer;
}; */

bool intPointInPath(IntPoint ip, Path p);
bool pathInpath(Path3D target, Path3D in);

vector<std::array<QVector3D, 3>> interpolate(Path3D from, Path3D to);

uint32_t intPoint2Hash(IntPoint u);
uint32_t meshVertex2Hash(MeshVertex u);

#endif // MESH_H
