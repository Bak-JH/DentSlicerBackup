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
    Paths getMeshRecArea(const Mesh& mesh);
    Paths getMeshConvexHull(const Mesh& mesh);
    Paths spreadingCheck(const Mesh* mesh, std::vector<bool>& check, int chking_start, bool is_chking_pos);
    int getPathHead(const MeshFace* mf, int side, bool is_chking_pos);
    Path buildOutline(const Mesh* mesh, std::vector<bool>& check, int chking, int path_head, bool is_chking_pos);
    bool isEdgeBound(const MeshFace* mf, int side, bool is_chking_pos);
    bool isNbrOrientSame(const MeshFace* mf, int side);
    int searchVtxInFace(const MeshFace* mf, int vertexIdx);
    vector<int> arrToVect(const int arr[]);
    int getNbrVtx(const MeshFace* mf, int base, int xth);
    Path idxsToPath(const Mesh* mesh, vector<int> path_by_idx);
    Paths project(const Mesh* mesh);
    Paths clipOutlines(vector<Paths> outline_sets);
    bool checkFNZ(const MeshFace* face, bool is_chking_pos);
    void debugPaths(Paths paths);
    void debugPath(Path path);
    void debugFaces(const Mesh* mesh, vector<int> face_list);
    void debugFace(const Mesh* mesh, int face_idx);
    int findVertexWithIntpoint(IntPoint p, const Mesh* mesh);
    int findVertexWithIntXY(int x, int y, const Mesh* mesh);

    void RDPSimpPaths(Paths* paths);
    void RDPSimp(Path* path);
    void RDPSimpIter(Path* path, int start, int end, std::vector<bool>& keepingPointMrkArr);
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


Paths3D spreadingCheckExt(const Mesh& mesh, int chking_start);
int getExtPathHead(const MeshFace& mf, int side);
Path3D buildOutlineExt(const Mesh& mesh, std::vector<bool>& outer_check, int chking, int path_head);
int getNbrVtxExt(const MeshFace& mf, int base, int xth);
bool isEdgeBoundExt(const MeshFace& mf, int side);
bool isNbrOrientSameExt(const MeshFace& mf, int side);
bool meetNbrCondExt(const MeshFace& mf);
Path3D idxsToPathExt(const Mesh& mesh, vector<int> path_by_idx);
int searchVtxInFaceExt(const MeshFace& mf, int vertexIdx);
vector<int> arrToVectExt(const int arr[]);

#endif // ARRANGE_H
