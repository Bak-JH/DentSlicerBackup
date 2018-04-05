#ifndef KRAFT_H
#define KRAFT_H
#include "DentEngine/src/polyclipping/clipper.hpp"
#include "DentEngine/src/configuration.h"
#include "DentEngine/src/support.h"

using namespace std;
using namespace ClipperLib;

class Slice;
class Slices;

class KRaft{
public:
    KRaft();
};

void generateKRaft(Slices& slices);


/****************** Helper Functions *******************/

#endif // KRAFT_H
