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

    vector<vector<Path>> slice(Mesh* mesh);

    vector<vector<Path>> meshSlice(Mesh* mesh); // totally k elements
    vector<Path> contourConstruct(vector<Path>);

    /****************** Helper Functions For Mesh Slicing Step *******************/

    vector<vector<int>> buildTriangleLists(Mesh* mesh, vector<float> planes, float delta);
    vector<float> buildUniformPlanes(float z_min, float z_max, float delta);
    vector<float> buildAdaptivePlanes(float z_min, float z_max);


    /****************** Helper Functions For Contour Construction Step *******************/
    void insertPathHash(QHash<int64_t, Path> *pathHash, IntPoint u, IntPoint v);
    int64_t intPoint2Hash(IntPoint u);
};


#endif // SLICER_H
