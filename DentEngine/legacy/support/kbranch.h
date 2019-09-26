#ifndef KBRANCH_H
#define KBRANCH_H
#include "../polyclipping/clipper/clipper.hpp"
#include "../configuration.h"
#include "../support.h"
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
