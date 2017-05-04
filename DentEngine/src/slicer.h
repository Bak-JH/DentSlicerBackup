#ifndef SLICER_H
#define SLICER_H
#include "polyclipping/clipper.hpp"
#include "configuration.h"
#include "mesh.h"
#include <list>
#include <qDebug>

using namespace std;
using namespace ClipperLib;

class Slicer : public Clipper
{
public:
    Slicer(Configuration* cfg) : cfg(cfg) {}
    Configuration* cfg;

    int slice(Mesh* mesh);

    vector<vector<Path>> meshSlice(Mesh* mesh); // totally k elements
    int contourConstruct(vector<vector<Path>>);
    vector<vector<int>> buildTriangleLists(Mesh* mesh, vector<float> planes, float delta);
    vector<float> buildUniformPlanes(float z_min, float z_max, float delta);
    vector<float> buildAdaptivePlanes(float z_min, float z_max);
};


#endif // SLICER_H
