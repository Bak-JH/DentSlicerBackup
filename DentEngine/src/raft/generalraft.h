#ifndef GENERALRAFT_H
#define GENERALRAFT_H
#include "DentEngine/src/polyclipping/clipper.hpp"
#include "DentEngine/src/configuration.h"
#include "DentEngine/src/raft.h"

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
