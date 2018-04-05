#ifndef GENERALRAFT_H
#define GENERALRAFT_H
#include "../polyclipping/clipper.hpp"
#include "../configuration.h"
#include "../raft.h"

using namespace std;
using namespace ClipperLib;

class Slice;
class Slices;

class GeneralRaft
{
public:
    GeneralRaft();
};

void generateGeneralRaft(Slices& slices);

/****************** Helper Functions *******************/

#endif // GENERALRAFT_H
