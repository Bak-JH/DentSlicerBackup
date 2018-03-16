#ifndef ARRANGE_H
#define ARRANGE_H
#include <vector>
#include <QVector3D>
#include <Qt3DCore>
#include <QHash>
#include <QDebug>
#include <math.h>
#include "./configuration.h"
#include "./mesh.h"
#include "./polyclipping/clipper.hpp"
#include "convex_hull.h"

using namespace std;
using namespace ClipperLib;

class Arrange
{
public:
    Arrange();
};

Paths spreadingCheck(Mesh* mesh, bool* check, int chking_start, bool is_chking_pos);
int getPathHead(MeshFace* mf, int side, bool is_chking_pos);
Path buildOutline(Mesh* mesh, bool* check, int chking, int path_head, bool is_chking_pos);
bool isEdgeBound(MeshFace* mf, int side, bool is_chking_pos);
bool isNbrOrientSame(MeshFace* mf, int side);
int searchVtxInFace(MeshFace* mf, int vertexIdx);
vector<int> arrToVect(int arr[]);
int getNbrVtx(MeshFace* mf, int base, int xth);
Path idxsToPath(Mesh* mesh, vector<int> path_by_idx);
Paths project(Mesh* mesh);
Paths clipOutlines(vector<Paths> outline_sets);
bool checkFNZ(MeshFace* face, bool is_chking_pos);
void debugPaths(Paths paths);
void debugPath(Path path);
void debugFaces(Mesh* mesh, vector<int> face_list);
void debugFace(Mesh* mesh, int face_idx);
int findVertexWithIntpoint(IntPoint p, Mesh* mesh);
int findVertexWithIntXY(int x, int y, Mesh* mesh);

void RDPSimpPaths(Paths* paths);
void RDPSimp(Path* path);
void RDPSimpIter(Path* path, int start, int end, bool* keepingPointMrkArr);
float distL2P(IntPoint* line_p1, IntPoint* line_p2, IntPoint* p);

typedef pair<IntPoint, float> XYArrangement;
vector<XYArrangement> arngMeshes(vector<Mesh>* meshes);
vector<XYArrangement> arng2D(vector<Paths>* figs);
void initStage(Paths* cum_outline);
XYArrangement arngFig(Paths* cum_outline, Paths* fig);
Paths rotatePaths(Paths* paths, float rot_angle);
IntPoint rotateIntPoint(IntPoint point, float rot_angle);
void tanslatePaths(Paths* paths, IntPoint tanslate_vec);
IntPoint tanslateIntPoint(IntPoint point, IntPoint tanslate_vec);
void cumulativeClip(Paths* cum_outline, Paths* new_fig);
IntPoint getOptimalPoint(Paths* nfp_set);
int getMaxX(IntPoint translate_vec, Paths* fig);
int getMaxY(IntPoint translate_vec, Paths* fig);
Paths getNFP(Paths* subject, Paths* object);
Paths simplyfyRawNFP(Paths* raw_nfp_set, Paths* subject);
vector<vector<IntPoint>> getObjVecsInRegions(vector<float>* sub_slope_set, vector<float>* obj_slope_set, vector<IntPoint>* obj_vec_set);
bool isOnCCWPath(float start, float end, float object);
float getNthEdgeSlope(Path* path, int edge_idx, bool isForward);
float getEdgeSlope(IntPoint* p1, IntPoint* p2);
IntPoint getNthEdgeVec(Path* path, int edge_idx, bool isForward);
IntPoint getEdgeVec(IntPoint* p1, IntPoint* p2);
IntPoint getFirstNFPPoint(float first_slope, IntPoint first_point, Path* obj, vector<float> obj_slope_set);
IntPoint rotateIntPointRad(IntPoint point, float rot_angle);

void arrangeQt3D(vector<Qt3DCore::QTransform*> m_transform_set, vector<XYArrangement> arng_result_set);
void arrangeSingleQt3D(Qt3DCore::QTransform* m_transform, XYArrangement arng_result);

void testSimplifyPolygon();
void testClip();

#endif // ARRANGE_H
