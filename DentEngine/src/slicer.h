#ifndef SLICER_H
#define SLICER_H
#include "mesh.h"
//#include "support.h"
//#include "infill.h"
#include <list>
#include <QThread>
#include <QDebug>
using namespace ClipperLib;
using namespace Hix::Engine3D;
class OverhangPoint;


struct ContourSegment
{
	ContourSegment();
	//constructor helper;
	void init(const QVector3D& a, const QVector3D& b, FaceConstItr& face);
	//two points
	ContourSegment(VertexConstItr& vtxA, VertexConstItr& vtxB, FaceConstItr& face );
	//one point(B) and an edge(A0->A1)
	ContourSegment(VertexConstItr& vtxA0, VertexConstItr& vtxA1, VertexConstItr& vtxB, float z, FaceConstItr& face);
	//two edges
	ContourSegment(VertexConstItr& vtxA0, VertexConstItr& vtxA1, VertexConstItr& vtxB0, VertexConstItr& vtxB1, float z, FaceConstItr& face);
	bool isValid();
	
	//ordering is important.
	//follows Righ hand thumb finger rule, if the in, goint to->from normal vector is pointed left side, CW 
	IntPoint to;
	IntPoint from;
	DoublePoint normal;
};

class Contour
{
public:
	Contour(const ContourSegment* start);
	bool isClosed();
	IntPoint getDestination();
	void addNext(const ContourSegment* seg);
	void calculateDirection();
	bool isOutward();
private:
	void checkBound(const IntPoint& pt);
	//bounds
	cInt _xMax = std::numeric_limits<cInt>::lowest();
	cInt _xMin = std::numeric_limits<cInt>::max();
	cInt _yMax = std::numeric_limits<cInt>::lowest();
	cInt _yMin = std::numeric_limits<cInt>::max();
	
	std::deque<ContourSegment> segments;
	bool _isOutward = false;
};


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
}

class Slice{ // extends Paths (total paths)
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


class Slices : public std::vector<Slice>{
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

namespace ContourGen
{
	std::vector<Contour> contourConstruct(const std::vector<ContourSegment>& segments);

}

namespace Slicer
{
	void addPoint(float x, float y, ClipperLib::Path* path);
    /****************** Entire Slicing Step *******************/
    bool slice(const Mesh* mesh, Slices* slices);
};


/****************** ZFill method on intersection point *******************/
void zfillone(IntPoint& e1bot, IntPoint& e1top, IntPoint& e2bot, IntPoint& e2top, IntPoint& pt);


#endif // SLICER_H
