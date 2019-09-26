#ifndef RAFT_H
#define RAFT_H
#include "slicer.h"
#include "polyclipping/clipper/clipper.hpp"
#include "raft/kraft.h"
#include "raft/generalraft.h"

#define generalraft 1
#define kraft 2

using namespace std;
using namespace ClipperLib;
class Slice;
class Slices;



class Raft
{
public:
    int type;

    Raft(int type);
    void generate(Slices& slices);
};

#endif // RAFT_H
