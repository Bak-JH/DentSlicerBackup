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
class MeshVertex;
class Mesh;


class MeshDataType {
public:
	MeshDataType(const Mesh* owner) :Owner(owner)
	{}
private:
	virtual MeshDataType* modifiedByOwner(const Mesh* owner)const = 0;
protected:
	const Mesh* Owner;

};
class MeshFace: private MeshDataType {
public:
    int idx;
	const MeshFace* parentFace = nullptr;
    QVector3D fn;
    QVector3D fn_unnorm;

	std::array<std::vector<const MeshFace*>, 3> neighboring_faces;
	std::array<const MeshVertex*, 3> mesh_vertex{ nullptr, nullptr, nullptr };
private:
	MeshFace(Mesh* mesh) : neighboring_faces{ std::vector<const MeshFace*>(), std::vector<const MeshFace*>(), std::vector<const MeshFace*>() }, MeshDataType(mesh), idx(-1)
	{}
	std::list<MeshFace>::iterator itr;
	MeshFace* modifiedByOwner(const Mesh* owner)const override
	{
		if (owner == Owner)
		{
			return const_cast<MeshFace*>(this);
		}
		return nullptr;
	}

	friend class Mesh;
};

class MeshVertex : private MeshDataType {
public:
	int idx;
    QVector3D position;
    QVector3D vn;
	MeshVertex():MeshDataType(nullptr), idx(-1) {}

    friend inline bool operator== (const MeshVertex& a, const MeshVertex& b){
        return a.position == b.position;
    }

    friend inline bool operator!= (const MeshVertex& a, const MeshVertex& b){
        return a.position != b.position;
    }

	std::vector<const MeshFace*> connected_faces;

private:
	MeshVertex(Mesh* mesh, QVector3D position) : MeshDataType(mesh), position(position) {}
	MeshVertex(Mesh* mesh) : MeshDataType(mesh) {}
	std::list<MeshVertex>::iterator itr;
	MeshVertex* modifiedByOwner(const Mesh* owner)const override
	{
		if (owner == Owner)
		{
			return const_cast<MeshVertex*>(this);
		}
		return nullptr;
	}

	friend class Mesh;
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
	//enum ForEachOperation {
	//	Delete = 0,
	//	Modify
	//};
	////For each operation, checks and returns if the given face qualifies for delete/modify
	//typedef std::function<bool(const Mesh&, const MeshFace&)> FaceCondtionalFunction;
	////For each operation, checks and returns if the given vertex qualifies for delte/modify
	//typedef std::function<bool(const Mesh&, const MeshVertex&)> VertexConditionalFunction;

	//For each operation, modifies MeshFace, returns whether modification actually occured or the element should be delete
	typedef std::function<bool(const Mesh&, MeshFace&, size_t)> FaceForEachFunction;
	typedef std::function<bool(const Mesh&, MeshVertex&, size_t)> VertexForEachFunction;

	Mesh(const Mesh* origin);
    Mesh() {};
    /********************** Mesh Edit Functions***********************/
    void vertexOffset(float factor);
    void vertexMove(QVector3D direction);
    void centerMesh();
    void vertexRotate(QMatrix4x4 tmpmatrix);
    void vertexScale(float scaleX, float scaleY, float scaleZ, float centerX, float centerY);
    void reverseFaces();
	void addFace(QVector3D v0, QVector3D v1, QVector3D v2);
	void addFace(QVector3D v0, QVector3D v1, QVector3D v2, const MeshFace* parentface);
	std::list<MeshFace>::const_iterator removeFace(std::list<MeshFace>::const_iterator f_it);
	void removeFace(const MeshFace* mf);
	void connectFaces();
	void modifyVertex(const MeshVertex* vertex, const QVector3D& newValue);
	Mesh* saveUndoState(const Qt3DCore::QTransform& transform);
	//void modifyFace(const MeshFace* vertex, const QVector3D& newValue);

	/********************** Faces & Vertices std::for_each style edit***********************/
	//returns changed/deleted elements count
	//size_t doForEachFace(ForEachOperation operationType, FaceForEachFunction forEachFunction);
	//size_t doForEachVertex(ForEachOperation operationType, VertexForEachFunction forEachFunction);

	size_t conditionalDelteFaces(FaceForEachFunction forEachFunction);
	size_t conditionalModifyFaces(FaceForEachFunction forEachFunction);

	size_t conditionalDelteVertices(VertexForEachFunction forEachFunction);
	size_t conditionalModifyVertices(VertexForEachFunction forEachFunction);



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
	static std::array<float, 6> calculateMinMax(QMatrix4x4 rotmatrix, const Mesh& mesh);

    float getFaceZmin(MeshFace mf)const;
    float getFaceZmax(MeshFace mf)const;
    //MeshFace idx2MF(int idx)const;
    //MeshVertex idx2MV(int idx)const;

    /********************** Getters **********************/
    const std::list<MeshVertex>* getVertices()const;
    const std::list<MeshFace>* getFaces()const;
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
    std::vector<const MeshFace*> findFaceWith2Vertices(const MeshVertex * v0, const MeshVertex* v1,const MeshFace& self_f) const;
	MeshVertex* addFaceVertex(QVector3D v);
    void updateMinMax(QVector3D v);

    std::list<MeshVertex> vertices;
    QHash<uint32_t, MeshVertex*> vertices_hash;
    std::list<MeshFace> faces;

    // for undo & redo
    Mesh* prevMesh = nullptr;
    Mesh* nextMesh = nullptr;

    QVector3D m_translation;
    QMatrix4x4 m_matrix;
    QTime time;

    float _x_min = 99999, _x_max = 99999, _y_min = 99999, _y_max = 99999, _z_min = 99999, _z_max = 99999;

	//fileloader should be a factory pattern?
    friend class FileLoader;
    friend class GLModel;





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
