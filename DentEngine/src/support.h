#ifndef SUPPORT_H
#define SUPPORT_H
#include "polyclipping/clipper.hpp"
#include "configuration.h"
#include "mesh.h"
#include "support/kbranch.h"
#include <list>


using namespace std;
using namespace ClipperLib;

class SupportBase
{
public:
    SupportBase(float x, float y, float z);
    vector<float> start; // x, y, z
    vector<float> end; // x, y, z
    vector<SupportBase*> parents; // contains at most 2 parents
    vector<SupportBase*> child; // contains child branches brached off from this

    void setStartPosition(float x, float y, float z);
    void setEndPosition(float x, float y, float z);
};

class Support
{
public:
    Support(Configuration* cfg) : cfg(cfg) {}
    Configuration* cfg;

    vector<SupportBase> overhangDetect();

};


#endif // SUPPORT_H
