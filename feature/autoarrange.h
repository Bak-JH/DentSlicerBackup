#ifndef ARRANGE_H
#define ARRANGE_H
#include <vector>
#include <algorithm>
#include <QVector3D>
#include <Qt3DCore>
#include <QHash>
#include <QDebug>
#include <math.h>
//#include "./glmodel.h"
#include "DentEngine/src/configuration.h"
#include "DentEngine/src/mesh.h"
#include "DentEngine/src/polyclipping/clipper/clipper.hpp"
#include "convex_hull.h"

using namespace std;
using namespace ClipperLib;

typedef pair<IntPoint, float> XYArrangement;
typedef vector<IntPoint> Vecs;

class autoarrange : public QObject {
    Q_OBJECT
public:
    autoarrange();

    vector<XYArrangement> simpArngMeshes(vector<Mesh>& meshes);
    vector<XYArrangement> arngMeshes(vector<Mesh>& meshes);
    void arrangeQt3D(vector<Qt3DCore::QTransform*> m_transform_set, vector<XYArrangement> arng_result_set);
    //void arrangeGlmodels(vector< GLModel* > * glmodels);

private:
    Paths getMeshRecArea(Mesh& mesh);
    Paths getMeshConvexHull(Mesh& mesh);
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

    void offsetPath(Paths* paths);
    void offsetPaths_rec(Paths& paths);

    vector<XYArrangement> arng2D(vector<Paths>& figs);
    void initStage(Paths& cum_outline);
    XYArrangement newArngFig(Paths& cum_outline, Paths& fig);
    XYArrangement arngFig(Paths& cum_outline, Paths& fig);
    Paths rotatePaths(Paths& paths, float rot_angle);
    IntPoint rotatePoint(IntPoint& point, float rot_angle);
    void tanslatePaths(Paths& paths, IntPoint& tanslate_vec);
    IntPoint tanslatePoint(IntPoint& point, IntPoint& tanslate_vec);
    void cumulativeClip(Paths& cum_outline, Paths& new_fig);
    IntPoint getOptimalPoint(Paths& nfp_set);
    int getMaxX(Path& path);
    int getMinX(Path& path);
    int getMaxY(Path& path);
    int getMinY(Path& path);
    Paths getNFP(Paths& subject, Paths& object);
    Paths simplyfyRawNFP(Paths& raw_nfp_set, Paths& subject);
    Paths mergeNFP(Paths& separate_nfp_set);
    vector<Vecs> getObjVecsInRegions(Vecs& sub_vecs, Vecs& obj_vecs);
    bool isVecAlign(const IntPoint& a, const IntPoint& b);
    bool isCCW(const IntPoint& a, const IntPoint& b, const IntPoint& c);
    bool isVecCCW(const IntPoint& a, const IntPoint& b, const IntPoint& c);
    IntPoint getEdgeVec(const Path& path, int edge_idx, bool isForward);
    IntPoint getFirstNFPPoint(const IntPoint& first_sub_vec, const IntPoint& first_sub_vtx, const Path& obj, const Vecs& obj_vecs);

    void arrangeSingleQt3D(Qt3DCore::QTransform* m_transform, XYArrangement arng_result);

    bool checkConvex(Path& path);

    void testSimplifyPolygon();
    void testClip();
    void testOffset();

};

bool compareArea(pair<Paths*, int>& fig1, pair<Paths*, int>& fig2);


Paths3D spreadingCheckExt(Mesh& mesh, int chking_start);
int getExtPathHead(MeshFace& mf, int side);
Path3D buildOutlineExt(Mesh& mesh, bool* outer_check, int chking, int path_head);
int getNbrVtxExt(MeshFace& mf, int base, int xth);
bool isEdgeBoundExt(MeshFace& mf, int side);
bool isNbrOrientSameExt(MeshFace& mf, int side);
bool meetNbrCondExt(MeshFace& mf);
Path3D idxsToPathExt(Mesh& mesh, vector<int> path_by_idx);
int searchVtxInFaceExt(MeshFace& mf, int vertexIdx);
vector<int> arrToVectExt(int arr[]);

#endif // ARRANGE_H
