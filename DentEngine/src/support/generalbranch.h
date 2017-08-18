#ifndef GENERALBRANCH_H
#define GENERALBRANCH_H
#include "src/polyclipping/clipper.hpp"
#include "src/configuration.h"
#include "src/support.h"

using namespace std;
using namespace ClipperLib;

class Slice;
class Slices;
class OverhangPoint;

class GeneralBranch
{
public:
    GeneralBranch();
};


void generateGeneralBranch(Slices& slices);
/****************** Helper Functions *******************/
float pointDistance(IntPoint A, IntPoint B);
float pointDistance(QVector3D A, QVector3D B);
bool checkInclusion(Slice& slice, OverhangPoint overhang_point);
Path drawCircle(OverhangPoint overhang_point, int radius);

#endif // GENERALBRANCH_H
