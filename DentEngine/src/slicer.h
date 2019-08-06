#pragma once
#include "mesh.h"
//#include "support.h"
//#include "infill.h"
#include <list>
#include <QThread>
#include <QDebug>
#include <QVector2D>
using namespace ClipperLib;
using namespace Hix::Engine3D;
class OverhangPoint;


namespace std
{
	template<>
	struct hash<IntPoint>
	{
		//2D only!
		std::size_t operator()(const IntPoint& pt)const
		{
			int16_t smallX = (int16_t)(pt.X);
			int16_t smallY = (int16_t)(pt.Y);
			size_t digest = smallX | smallY << 16;
			return digest;
		}
	};
	template<>
	struct hash<std::pair<VertexConstItr, VertexConstItr>>
	{
		//2D only!
		std::size_t operator()(const std::pair<VertexConstItr, VertexConstItr>& pair)const
		{
			//we don't care about symmertricity as the vertice pair should be sorted anyway ie) no symmetry
			return std::hash<VertexConstItr>()(pair.first) ^ std::hash<VertexConstItr>()(pair.second);
		}
	};
}


class Slice { // extends Paths (total paths)
public:
	float z;
	PolyTree polytree; // containment relationship per slice
	Paths overhang_region;
	Paths critical_overhang_region;

	Paths outershell;
	Paths infill;
	Paths support;

	int Area;

	void outerShellOffset(float delta, JoinType join_type);
};


class Slices : public std::vector<Slice> {
public:
	const Mesh* mesh;

	Paths overhang_regions;
	std::vector<OverhangPoint*> overhang_points;
	//    std::vector<QVector3D> overhang_points;
	//std::vector<IntPoint> intersectionPoints;
	Path raft_points;

	void containmentTreeConstruct();

};
namespace Hix
{
	namespace Debug
	{
		QDebug operator<< (QDebug d, const Slice& obj);
		QDebug operator<< (QDebug d, const Slices& obj);
	}
}



namespace Slicer
{

	enum class NodeState : uint8_t
	{
		NotUsed = 0,
		Unexplored = 1,
		Explored = 2
	};

	/****************** Helper Functions For Mesh Slicing Step *******************/
	std::vector<std::unordered_set<FaceConstItr>>  buildTriangleLists(const Mesh* mesh, std::vector<float> planes, float delta);
	std::vector<float> buildUniformPlanes(float z_min, float z_max, float delta);
	std::vector<float> buildAdaptivePlanes(float z_min, float z_max);

	void addPoint(float x, float y, ClipperLib::Path* path);
	/****************** Entire Slicing Step *******************/
	bool slice(const Mesh* mesh, Slices* slices);
};

struct ContourSegment
{
	ContourSegment();
	//constructor helper;
	bool isValid();
	bool calcNormalAndFlip();
	float dist();
	//
	//ordering is important.
	//follows Righ hand thumb finger rule, if the in, goint to->from normal vector is pointed left side, CW 
	bool unknownDirection = false;
	FaceConstItr face;
	QVector2D from;
	QVector2D to;
	QVector2D normal;
};

class Contour
{
public:
	//Contour(const ContourSegment* start);
	bool isClosed();
	//IntPoint getDestination();
	void addNext(const ContourSegment& seg);
	//void calculateDirection();
	//bool isOutward();
private:
	//void checkBound(const IntPoint& pt);
	////bounds
	//cInt _xMax = std::numeric_limits<cInt>::lowest();
	//cInt _xMin = std::numeric_limits<cInt>::max();
	//cInt _yMax = std::numeric_limits<cInt>::lowest();
	//cInt _yMin = std::numeric_limits<cInt>::max();
		//bool _isOutward = false;
	bool _directionDetermined = false;
	std::deque<ContourSegment> segments;
};

//one per plane
class ContourBuilder
{
public:
	ContourBuilder(const Mesh* mesh, std::unordered_set<FaceConstItr>& intersectingFaces, float z);
	std::vector<Contour> buildContours();

private:
	//could use bool, just incase we need to resolve non-2-maifold

	//two points

	ContourSegment calculateStartingSegment(FaceConstItr& mf, std::variant<VertexConstItr, HalfEdgeConstItr>& hint);
	ContourSegment doNextSeg(VertexConstItr from, FaceConstItr prevFace, std::variant<VertexConstItr, HalfEdgeConstItr>& to);
	ContourSegment doNextSeg(HalfEdgeConstItr from, QVector2D fromPt, std::variant<VertexConstItr, HalfEdgeConstItr>& to);
	QVector2D midPoint2D(VertexConstItr vtxA0, VertexConstItr vtxA1);

	const Mesh* _mesh;
	float _plane;
	std::unordered_map<std::pair<VertexConstItr, VertexConstItr>, QVector2D> _midPtLUT;
	std::unordered_set<FaceConstItr>& _intersectList;
	std::unordered_set<FaceConstItr> _exploredList;
	std::unordered_set<FaceConstItr> _tooShortList;

};




/****************** ZFill method on intersection point *******************/
void zfillone(IntPoint& e1bot, IntPoint& e1top, IntPoint& e2bot, IntPoint& e2top, IntPoint& pt);

