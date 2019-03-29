#ifndef KRAFT_H
#define KRAFT_H
#include "../polyclipping/clipper/clipper.hpp"
#include "../configuration.h"
#include "../support.h"

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
