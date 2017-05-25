#ifndef SUPPORT_H
#define SUPPORT_H
#include "polyclipping/clipper.hpp"
#include "support/kbranch.h"

#define generalsupport 1
#define kbranchsupport 2

using namespace std;
using namespace ClipperLib;
class Slice;
class Slices;

class Support {
public:
    int type;

    Support(int type);
    void generate(Slices* slices);
};


#endif // SUPPORT_H
