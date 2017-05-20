#ifndef KBRANCH_H
#define KBRANCH_H
#include "src/polyclipping/clipper.hpp"

using namespace std;
using namespace ClipperLib;

class Slice;
using Slices = vector<Slice>;

class kbranch
{
public:
    kbranch();

};

void generateKbranch(Slices* slices);

#endif // KBRANCH_H
