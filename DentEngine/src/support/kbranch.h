#ifndef KBRANCH_H
#define KBRANCH_H
#include "src/polyclipping/clipper.hpp"
#include "src/configuration.h"
#include "src/support.h"

using namespace std;
using namespace ClipperLib;

class Slice;
class Slices;
class OverhangPosition;

class kbranch{
public:
    kbranch();


};

void generateKbranch(Slices& slices);
/****************** Helper Functions *******************/
bool checkInclusion(Slice& slice, OverhangPosition overhang_position);
Path drawCircle(OverhangPosition overhang_position);

#endif // KBRANCH_H
