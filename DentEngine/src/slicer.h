#ifndef SLICER_H
#define SLICER_H
#include "polyclipping/clipper.hpp"
#include "configuration.h"
#include "mesh.h"
#include "support.h"
#include "infill.h"
#include "raft.h"
#include <list>
#include <qDebug>

using namespace std;
using namespace ClipperLib;

class Slice : public Paths{ // extends Paths (multiple contours contained in one Slice)
public:
    float z;
    Paths overhang_region;
};

using Slices = vector<Slice>;

class Slicer : public Clipper
{
public:
    Slicer(Configuration* cfg) : cfg(cfg) {}
    Configuration* cfg;

    // entire slicing step
    Slices slice(Mesh* mesh);

    // contour slicing
    vector<Paths> meshSlice(Mesh* mesh); // totally k elements
    Slice contourConstruct(Paths);

    // overhang region detect
    void overhangDetect(Slices* contoursList);


    /****************** Helper Functions For Mesh Slicing Step *******************/

    vector<vector<int>> buildTriangleLists(Mesh* mesh, vector<float> planes, float delta);
    vector<float> buildUniformPlanes(float z_min, float z_max, float delta);
    vector<float> buildAdaptivePlanes(float z_min, float z_max);


    /****************** Helper Functions For Contour Construction Step *******************/
    void insertPathHash(QHash<int64_t, Path> *pathHash, IntPoint u, IntPoint v);
    int64_t intPoint2Hash(IntPoint u);
};



#endif // SLICER_H
