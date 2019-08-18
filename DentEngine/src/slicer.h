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
			size_t digest = pt.X | pt.Y << 48;
			return digest;
		}
	};

	template<>
	struct hash<QVector2D>
	{
		//2D only!
		std::size_t operator()(const QVector2D& pt)const
		{
			size_t x = (size_t)(pt.x());
			size_t y = (size_t)(pt.y());
			size_t digest = x | y << 16;
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
	PolyTree overhang;

	Paths outershell;
	Paths infill;
	Paths support;

	int Area;

	void outerShellOffset(float delta, JoinType join_type);
	void getOverhang(const Slice* prevSlice, PolyTree& result)const;
	void getOverhangMN(const Slice* prevSlice, std::unordered_set<const PolyNode*>& result)const;

};


class Slices : public std::vector<Slice> {
public:
	const Mesh* mesh;
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
	enum FlipResult : uint8_t
	{
		NotFlipped = 0,
		Flipped = 1,
		UnknownDirection = 2
	};
	ContourSegment();
	//constructor helper;
	bool isValid()const;
	FlipResult calcNormalAndFlip();
	float dist()const;
	void flip();
	//
	//ordering is important.
	//follows Righ hand thumb finger rule, if the in, goint to->from normal vector is pointed left side, CW 
	FaceConstItr face;
	QVector2D from;
	QVector2D to;
	QVector2D normal;
};

class Contour
{
public:
	//Contour(const ContourSegment* start);
	bool isClosed()const;
	//attempts to close the unclosed contour, if the gap is too large, give up.
	bool tryClose();
	//IntPoint getDestination();
	void addNext(const ContourSegment& seg);
	void addPrev(const ContourSegment& seg);
	float dist()const;
	QVector2D from()const;
	QVector2D to()const;
	void append(const Contour& appended);
	//void calculateDirection();
	//bool isOutward();
	std::deque<ContourSegment> segments;
	Path toPath()const;//tmp

private:
 	////bounds
	//cInt _xMax = std::numeric_limits<cInt>::lowest();
	//cInt _xMin = std::numeric_limits<cInt>::max();
	//cInt _yMax = std::numeric_limits<cInt>::lowest();
	//cInt _yMin = std::numeric_limits<cInt>::max();
		//bool _isOutward = false;
	bool _directionDetermined = false;
};

//wrapper for contour
class IncompleteContour
{
private:
	std::deque<const Contour*> _contours;
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

	bool isArea(const Contour& contour);
	void buildSegment(const FaceConstItr& mf);
	bool buildContour(Contour& current, bool reverse);
	QVector2D midPoint2D(VertexConstItr vtxA0, VertexConstItr vtxA1);
	std::unordered_set<Contour*> joinOrCloseIncompleteContours();

	const Mesh* _mesh;
	float _plane;
	std::unordered_map<std::pair<VertexConstItr, VertexConstItr>, QVector2D> _midPtLUT;
	const std::unordered_set<FaceConstItr>& _intersectList;
	std::unordered_map<FaceConstItr, ContourSegment> _segments;

	std::unordered_multimap<QVector2D, ContourSegment*> _fromHash;
	std::unordered_multimap<QVector2D, ContourSegment*> _toHash;
	std::unordered_multimap<QVector2D, ContourSegment*> _unknownHash;
	std::unordered_set<ContourSegment*> _unknownSegs;

	std::unordered_set<FaceConstItr> _unexplored;
	std::vector<Contour> _incompleteContours;

};




/****************** ZFill method on intersection point *******************/
void zfillone(IntPoint& e1bot, IntPoint& e1top, IntPoint& e2bot, IntPoint& e2top, IntPoint& pt);

