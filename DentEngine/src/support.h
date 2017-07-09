#ifndef SUPPORT_H
#define SUPPORT_H
#include "slicer.h"
#include "polyclipping/clipper.hpp"
#include "support/kbranch.h"

#define generalsupport 1
#define kbranchsupport 2

using namespace std;
using namespace ClipperLib;
class Slice;
class Slices;
class OverhangPosition;

class Support {
public:
    int type;

    Support(int type);
    void generate(Slices& slices);

};

/****************** Overhang Detection Step *******************/
void overhangDetect(Slices& slices);

/****************** Helper Functions For Overhang Detection Step *******************/
void getCriticalOverhangRegion(Slice& slice);
bool checkPerpendicularLength(Path A, Path B, IntPoint& left_hit);
float pointDistance(IntPoint A, IntPoint B);
float pointDistance(QVector3D A, QVector3D B);

IntPoint getPolygonNormal(Path vertices);
IntPoint getPolygonCentroid(Path vertices);
Paths areaSubdivision(Path area, float criterion);
void clusterPoints(vector<OverhangPosition>& points);





#endif // SUPPORT_H
