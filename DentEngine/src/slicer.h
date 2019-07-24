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
    Mesh* mesh;

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
    /****************** Entire Slicing Step *******************/
    bool slice(Mesh* mesh, Slices* slices);
};


/****************** ZFill method on intersection point *******************/
void zfillone(IntPoint& e1bot, IntPoint& e1top, IntPoint& e2bot, IntPoint& e2top, IntPoint& pt);


#endif // SLICER_H
