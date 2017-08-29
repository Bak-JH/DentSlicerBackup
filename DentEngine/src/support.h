#ifndef SUPPORT_H
#define SUPPORT_H
#include "slicer.h"
#include "polyclipping/clipper.hpp"
#include "support/kbranch.h"
#include "support/generalbranch.h"

#define generalsupport 1
#define kbranchsupport 2
#define PI 3.14159265

using namespace std;
using namespace ClipperLib;
class Slice;
class Slices;
class OverhangPoint;

class Support {
public:
    int type;

    Support(int type);
    void generate(Slices& slices);

};

class BranchableOverhangPoint {
public:
    OverhangPoint* overhangpoint;
    float distance;
};

class OverhangPoint {
public:
    OverhangPoint(int x=0, int y=0, int z=0, int radius=0) : position(x, y, z), radius(radius){}
    bool exist= true;
    IntPoint position;
    float height = 0; // in mm
    int radius;
    bool branchable = true;
    int branching_cnt = 0;
    OverhangPoint* branching_overhang_point;
    OverhangPoint* target_branching_overhang_point;
    IntPoint unit_move;
    vector<BranchableOverhangPoint*> branchable_overhang_points;

    void branchTo(IntPoint target);
};

/****************** Overhang Detection Step *******************/
void overhangDetect(Slices& slices);

/****************** Helper Functions For Overhang Detection Step *******************/
void getCriticalOverhangRegion(Slice& slice);
bool checkPerpendicularLength(Path A, Path B, IntPoint& left_hit);
float pointDistance(IntPoint A, IntPoint B);
float pointDistance(QVector3D A, QVector3D B);
bool checkInclusion(Slice& slice, OverhangPoint overhang_point);
Path drawCircle(OverhangPoint overhang_point, int radius);

IntPoint getPolygonNormal(Path vertices);
IntPoint getPolygonCentroid(Path vertices);
Paths areaSubdivision(Path area, float criterion);
void clusterPoints(vector<OverhangPoint>& points);





#endif // SUPPORT_H
