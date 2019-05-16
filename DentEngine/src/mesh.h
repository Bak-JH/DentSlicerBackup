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
#include <variant>
#include "../common/TrackedIndexedList.h"
#define cos50 0.64278761
#define cos100 -0.17364818
#define cos150 -0.8660254
#define FZERO 0.00001f
#if defined(_DEBUG) || defined(QT_DEBUG )
#endif
using namespace ClipperLib;

// plane contains at least 3 vertices contained in the plane in clockwise direction
typedef std::vector<QVector3D> Plane;
struct MeshVertex;
struct MeshEdge;
struct Mesh;

struct MeshFace {
	MeshFace(){}
    QVector3D fn;
    QVector3D fn_unnorm;
	//TODO: fix meshes with more than 3 faces that share an edge
	std::array<MeshEdge, 3> edges;
	std::array<size_t, 3> mesh_vertex;
};

struct MeshEdge {
	//increasing order
	std::array<size_t, 2> vertices;
	std::set<size_t> faces;
};

struct MeshVertex {

	MeshVertex() {}
	MeshVertex(QVector3D position) : position(position) {}
    friend inline bool operator== (const MeshVertex& a, const MeshVertex& b){
        return a.position == b.position;
    }
    friend inline bool operator!= (const MeshVertex& a, const MeshVertex& b){
        return a.position != b.position;
    }
	void calculateNormalFromFaces();
	QVector3D position;
	QVector3D vn;
	std::set<size_t> connected_faces;
	std::set<size_t> edges;
};

class Path3D : public std::vector<MeshVertex>{
public:
    Path projection;
    std::vector<Path3D> inner;
    std::vector<Path3D> outer;
};

typedef std::vector<Path3D> Paths3D;

class Mesh{
public :
	Mesh();
	Mesh(const Mesh* origin);

	/********************** Undo state functions***********************/
	void setNextMesh( Mesh* mesh);
	void setPrevMesh( Mesh* mesh);
	Mesh* saveUndoState(const Qt3DCore::QTransform& transform);


	/********************** Mesh Edit Functions***********************/
    void vertexOffset(float factor);
    void vertexMove(QVector3D direction);
    void centerMesh();
    void vertexRotate(QMatrix4x4 tmpmatrix);
    void vertexScale(float scaleX, float scaleY, float scaleZ, float centerX, float centerY);
    void reverseFaces();
	void addFace(QVector3D v0, QVector3D v1, QVector3D v2);
	void addFace(QVector3D v0, QVector3D v1, QVector3D v2, const MeshFace* parentface);
	TrackedIndexedList<MeshFace>::const_iterator removeFace(std::list<MeshFace>::const_iterator f_it);
	void connectFaces();
	TrackedIndexedList<MeshVertex>& getVertices();
	TrackedIndexedList<MeshFace>& getFaces();


	/********************** Mesh Modify and Copy Functions***********************/
	Mesh* copyMesh()const;
	Mesh* vertexMoved(QVector3D direction)const;


    /********************** Path Generation Functions **********************/
    static void addPoint(float x, float y, Path *path);
    Paths3D intersectionPaths(Path Contour, Plane target_plane)const;
    Path3D intersectionPath(Plane base_plane, Plane target_plane)const;
    Path intersectionPath(MeshFace mf, float z)const;

    /********************** Helper Functions **********************/
    static void updateMinMax(QVector3D v, std::array<float,6>& minMax);
	static std::array<float, 6> calculateMinMax(QMatrix4x4 rotmatrix, const Mesh* mesh);

    float getFaceZmin(MeshFace mf)const;
    float getFaceZmax(MeshFace mf)const;
    //MeshFace idx2MF(int idx)const;
    //MeshVertex idx2MV(int idx)const;

    /********************** Getters **********************/
	//const getter
    const TrackedIndexedList<MeshVertex>& getVertices()const;
    const TrackedIndexedList<MeshFace>& getFaces()const;
    float x_min()const;
    float x_max()const;
    float y_min()const;
    float y_max()const;
    float z_min()const;
    float z_max()const;
    Mesh* getPrev()const;
    Mesh* getNext()const;

	/********************** index to data **********************/

	inline MeshFace& idx2mf(size_t idx)
	{
		return faces[idx];
	}
	inline MeshVertex& idx2vtx(size_t idx)
	{
		return vertices[idx];
	}

	inline const MeshFace& idx2mf(size_t idx)const
	{
		return faces[idx];
	}
	inline const MeshVertex& idx2vtx(size_t idx)const
	{
		return vertices[idx];
	}

	/********************** Stuff that can be public **********************/

	QTime time;
	QVector3D m_translation;
	QMatrix4x4 m_matrix;


private:
    /********************** Helper Functions **********************/
    std::vector<const MeshFace*> findFaceWith2Vertices(const MeshVertex * v0, const MeshVertex* v1,const MeshFace& self_f) const;
	MeshVertex* addFaceVertex(QVector3D v);
    void updateMinMax(QVector3D v);

    TrackedIndexedList<MeshVertex> vertices;
    QHash<uint32_t, MeshVertex*> vertices_hash;
	TrackedIndexedList<MeshFace> faces;

    // for undo & redo
    Mesh* prevMesh = nullptr;
    Mesh* nextMesh = nullptr;

	//index changed event callback
	void vtxIndexChangedCallback(size_t oldIdx, size_t newIdx);
	void faceIndexChangedCallback(size_t oldIdx, size_t newIdx);

    float _x_min = 99999, _x_max = 99999, _y_min = 99999, _y_max = 99999, _z_min = 99999, _z_max = 99999;

	//fileloader should be a factory pattern?
    friend class FileLoader;
};


float round(float num, int precision);
bool doubleAreSame(double a, double b);

uint32_t vertexHash(QVector3D v);
float vertexDistance(QVector3D, QVector3D);

QHash<uint32_t, Path>::iterator findSmallestPathHash(QHash<uint32_t, Path> pathHash);

// construct closed contour using segments created from identify step
Paths contourConstruct(Paths);
Paths3D contourConstruct3D(Paths3D hole_edges);

bool intPointInPath(IntPoint ip, Path p);
bool pathInpath(Path3D target, Path3D in);

std::vector<std::array<QVector3D, 3>> interpolate(Path3D from, Path3D to);

uint32_t intPoint2Hash(IntPoint u);
uint32_t meshVertex2Hash(MeshVertex u);

#endif // MESH_H
