#ifndef KBRANCH_H
#define KBRANCH_H
#include "src/polyclipping/clipper.hpp"
#include "src/configuration.h"
#include "src/support.h"

using namespace std;
using namespace ClipperLib;

class Slice;
class Slices;

class kbranch{
public:
    kbranch();


    /****************** Helper Functions *******************/
    void drawCircle(QVector3D overhang_position);
};

void generateKbranch(Slices& slices);

#endif // KBRANCH_H
