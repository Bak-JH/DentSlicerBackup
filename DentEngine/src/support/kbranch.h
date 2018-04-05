#ifndef KBRANCH_H
#define KBRANCH_H
#include "DentEngine/src/polyclipping/clipper.hpp"
#include "DentEngine/src/configuration.h"
#include "DentEngine/src/support.h"
#include "DentEngine/src/util/convexhull.h"
#include <QVector3D>
#include <math.h>

using namespace std;
using namespace ClipperLib;

class Slice;
class Slices;
class OverhangPoint;

class KBranch{
public:
    KBranch();
};


void generateKBranch(Slices& slices);
/****************** Helper Functions *******************/
float pointDistance(IntPoint A, IntPoint B);
float pointDistance(QVector3D A, QVector3D B);
bool checkInclusion(Slice& slice, OverhangPoint overhang_point);
Path drawCircle(OverhangPoint overhang_point, int radius);

#endif // KBRANCH_H
