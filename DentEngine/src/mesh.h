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
#include <array>

#define cos50 0.64278761
#define cos100 -0.17364818
#define cos150 -0.8660254
#define FZERO 0.00001f

using namespace ClipperLib;

// plane contains at least 3 vertices contained in the plane in clockwise direction
typedef std::vector<QVector3D> Plane;

class MeshFace {
public:
	MeshFace() : neighboring_faces{ std::vector<size_t>(), std::vector<size_t>(), std::vector<size_t>() }
	{}

    int idx;
    int parent_idx = -1;
    std::array<int, 3> mesh_vertex{-1,-1,-1};
    //int connected_face_idx[3];

    std::array<std::vector<size_t>, 3> neighboring_faces;

    QVector3D fn;
    QVector3D fn_unnorm;
};

class MeshVertex {
public:
    MeshVertex(){}
    int idx;
    QVector3D position;
    QVector3D vn;
    std::vector<size_t> connected_faces;
    MeshVertex(QVector3D position): position(position) {}//connected_faces.reserve(8);}

    friend inline bool operator== (const MeshVertex& a, const MeshVertex& b){
        return a.position == b.position;
    }

    friend inline bool operator!= (const MeshVertex& a, const MeshVertex& b){
        return a.position != b.position;
    }
};

//typedef std::vector<MeshVertex> Path3D;
class Path3D : public std::vector<MeshVertex>{
    public:
        Path projection;
        std::vector<Path3D> inner;
        std::vector<Path3D> outer;
};

typedef std::vector<Path3D> Paths3D;

class Mesh{
public :
	Mesh(size_t vertCount, size_t faceCount, const Mesh* origin);
    Mesh() {};
    Mesh(size_t vertCount, size_t faceCount);
    /********************** Mesh Edit Functions***********************/
    void vertexOffset(float factor);
    void vertexMove(QVector3D direction);
    void centerMesh();
    void vertexRotate(QMatrix4x4 tmpmatrix);
    void vertexScale(float scaleX, float scaleY, float scaleZ, float centerX, float centerY);
    void reverseFaces();
	void setVertex(size_t index, const MeshVertex& value);

	/********************** Mesh Modify and Copy Functions***********************/
	Mesh* copyMesh()const;
	Mesh* vertexMoved(QVector3D direction)const;

    /********************** Mesh Generation Functions **********************/
    void addFace(QVector3D v0, QVector3D v1, QVector3D v2);
    void addFace(QVector3D v0, QVector3D v1, QVector3D v2, int parent_idx);
    std::vector<MeshFace>::iterator removeFace(std::vector<MeshFace>::iterator f_it);
    void removeFace(MeshFace* mf);
    void connectFaces();

    /********************** Path Generation Functions **********************/
    static void addPoint(float x, float y, Path *path);
    Paths3D intersectionPaths(Path Contour, Plane target_plane)const;
    Path3D intersectionPath(Plane base_plane, Plane target_plane)const;
    Path intersectionPath(MeshFace mf, float z)const;

    /********************** Helper Functions **********************/
    static void updateMinMax(QVector3D v, std::array<float,6>& minMax);

    std::array<float,6> calculateMinMax(QMatrix4x4 rotmatrix)const;
    float getFaceZmin(MeshFace mf)const;
    float getFaceZmax(MeshFace mf)const;
    MeshFace idx2MF(int idx)const;
    MeshVertex idx2MV(int idx)const;

    /********************** Getters **********************/
    const std::vector<MeshVertex>* getVertices()const;
    const std::vector<MeshFace>* getFaces()const;
    float x_min()const;
    float x_max()const;
    float y_min()const;
    float y_max()const;
    float z_min()const;
    float z_max()const;
    const Mesh* getPrev()const;
    const Mesh* getNext()const;
    QTime getTime()const;
private:
    /********************** Helper Functions **********************/
    std::vector<size_t> findFaceWith2Vertices(size_t v0_idx, size_t v1_idx, MeshFace self_f);
    int addFaceVertex(QVector3D v);
    void updateMinMax(QVector3D v);

    std::vector<MeshVertex> vertices;
    QHash<uint32_t, MeshVertex> vertices_hash;
    std::vector<MeshFace> faces;

    // for undo & redo
    Mesh* prevMesh = nullptr;
    Mesh* nextMesh = nullptr;

    QVector3D m_translation;
    QMatrix4x4 m_matrix;
    QTime time;



    // used for freecut, autoarrange elsewhere
    Path convexHull;

    float _x_min = 99999, _x_max = 99999, _y_min = 99999, _y_max = 99999, _z_min = 99999, _z_max = 99999;

    friend class FileLoader;
    friend class GLModel;
    //TODO: these friends should be removed so that operations on Mesh is controlled.
    friend class SVGexporter;
    friend class modelcut;
    friend class autoorientation;
    friend class modelcut;

	friend class GenerateSupport;






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
    std::vector<containmentPath> inner;
    std::vector<containmentPath> outer;
}; */

bool intPointInPath(IntPoint ip, Path p);
bool pathInpath(Path3D target, Path3D in);

std::vector<std::array<QVector3D, 3>> interpolate(Path3D from, Path3D to);

uint32_t intPoint2Hash(IntPoint u);
uint32_t meshVertex2Hash(MeshVertex u);

#endif // MESH_H
