#ifndef INFILL_H
#define INFILL_H
#include "polyclipping/clipper.hpp"


#define gridfill 1
#define honeycombfill 2
#define rhombicdodecahedronfill 3
#define octahedronfill 4

using namespace std;
using namespace ClipperLib;
class Slice;
using Slices = vector<Slice>;

class Infill
{
public:
    Infill(int type);
    void generate(Slices* slices);
};

#endif // INFILL_H
