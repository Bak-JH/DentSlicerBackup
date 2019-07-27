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
	ContourSegment(VertexConstItr& vtxA, VertexConstItr& vtxB, FaceConstItr& face );
	ContourSegment(VertexConstItr& vtxA0, VertexConstItr& vtxA1, VertexConstItr& vtxB, float z, FaceConstItr& face);
	ContourSegment(VertexConstItr& vtxA0, VertexConstItr& vtxA1, VertexConstItr& vtxB0, VertexConstItr& vtxB1, float z, FaceConstItr& face);
	
	//ordering is important.
	//follows Righ hand thumb finger rule, if the in, goint to->from normal vector is pointed left side, CW 
	IntPoint to;
	IntPoint from;
	DoublePoint normal;
};

struct Contour
{
	bool isClosed();
	void calculateDirection();
	std::deque<ContourSegment> segments;
	bool isOutward = false;
};

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

namespace Slicer
{
	void addPoint(float x, float y, ClipperLib::Path* path);
    /****************** Entire Slicing Step *******************/
    bool slice(const Mesh* mesh, Slices* slices);
};


/****************** ZFill method on intersection point *******************/
void zfillone(IntPoint& e1bot, IntPoint& e1top, IntPoint& e2bot, IntPoint& e2top, IntPoint& pt);


#endif // SLICER_H
