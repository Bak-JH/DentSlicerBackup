#ifndef RAFT_H
#define RAFT_H
#include "polyclipping/clipper.hpp"

#define generalraft 1
#define clampraft

using namespace std;
using namespace ClipperLib;
class Slice;
class Slices;



class Raft
{
public:
    Raft(int type);
    void generate(Slices* slices);
};

#endif // RAFT_H
