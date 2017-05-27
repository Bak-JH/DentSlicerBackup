#ifndef SLICER_H
#define SLICER_H
#include "polyclipping/clipper.hpp"
#include "configuration.h"
#include "mesh.h"
#include "support.h"
#include "infill.h"
#include "raft.h"
#include <list>
#include <QThread>
#include <qDebug>


using namespace std;
using namespace ClipperLib;

class Slice : public Paths{ // extends Paths (total paths)
public:
    float z;
    PolyTree polytree; // containment relationship per slice
    Paths overhang_region;
    Paths critical_overhang_region;

    Paths outershell;
    Paths infill;
    Paths support;

    void outerShellOffset(float delta, JoinType join_type);
};

class Slices : public vector<Slice>{
public:
    vector<QVector3D> overhang_positions;
};

class Slicer : public QThread
{
public:
    Slicer(Configuration* cfg) : cfg(cfg) {}
    Configuration* cfg;
    Slices slices;

    /****************** Entire Slicing Step *******************/
    Slices slice(Mesh* mesh);


    /****************** Mesh Slicing Step *******************/
    vector<Paths> meshSlice(Mesh* mesh); // totally k elements


    /****************** Contour Construction Step *******************/
    Paths contourConstruct(Paths);


    /****************** Overhang Detection Step *******************/
    void overhangDetect();


    /****************** Helper Functions For Mesh Slicing Step *******************/
    vector<vector<int>> buildTriangleLists(Mesh* mesh, vector<float> planes, float delta);
    vector<float> buildUniformPlanes(float z_min, float z_max, float delta);
    vector<float> buildAdaptivePlanes(float z_min, float z_max);


    /****************** Helper Functions For Contour Construction Step *******************/
    void insertPathHash(QHash<int64_t, Path>& pathHash, IntPoint u, IntPoint v);
    int64_t intPoint2Hash(IntPoint u);


    /****************** Helper Functions For Contour Management *******************/
    void getCriticalOverhangRegion(Slice& slice);
    bool checkPerpendicularLength(Path A, Path B, IntPoint& left_hit);


    /****************** Deprecated functions *******************/
    void overhangPositionPoll();
    void containmentTreeConstruct();
};


/****************** ZFill method on intersection point *******************/
void zfillone(IntPoint& e1bot, IntPoint& e1top, IntPoint& e2bot, IntPoint& e2top, IntPoint& pt);


#endif // SLICER_H
