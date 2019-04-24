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

using namespace ClipperLib;

typedef std::pair<IntPoint, float> XYArrangement;
typedef std::vector<IntPoint> Vecs;

class autoarrange : public QObject {
    Q_OBJECT
public:
    autoarrange();

    std::vector<XYArrangement> simpArngMeshes(std::vector<Mesh>& meshes);
    std::vector<XYArrangement> arngMeshes(std::vector<Mesh>& meshes);
    void arrangeQt3D(std::vector<Qt3DCore::QTransform*> m_transform_set, std::vector<XYArrangement> arng_result_set);
    //void arrangeGlmodels(std::vector< GLModel* > * glmodels);

private:
    Paths getMeshRecArea(const Mesh& mesh);
    Paths getMeshConvexHull(const Mesh& mesh);
    Paths spreadingCheck(const Mesh* mesh, std::vector<bool>& check, size_t chking_start, bool is_chking_pos);
	size_t getPathHead(const Mesh* mesh, const MeshFace* mf, size_t side, bool is_chking_pos);
    Path buildOutline(const Mesh* mesh, std::vector<bool>& check, size_t chking, size_t path_head, bool is_chking_pos);
    bool isEdgeBound(const Mesh* mesh, const MeshFace* mf, size_t side, bool is_chking_pos);
    bool isNbrOrientSame(const Mesh* mesh, const MeshFace* mf, size_t side);
	size_t searchVtxInFace(const MeshFace* mf, size_t vertexIdx);
	size_t getNbrVtx(const MeshFace* mf, size_t base, size_t xth);
    Path idxsToPath(const Mesh* mesh, std::vector<size_t> path_by_idx);
    Paths project(const Mesh* mesh);
    Paths clipOutlines(std::vector<Paths> outline_sets);
    bool checkFNZ(const MeshFace* face, bool is_chking_pos);
    void debugPaths(Paths paths);
    void debugPath(Path path);
    void debugFaces(const Mesh* mesh, std::vector<size_t> face_list);
    void debugFace(const Mesh* mesh, size_t face_idx);
	size_t findVertexWithIntpoint(IntPoint p, const Mesh* mesh);
	size_t findVertexWithIntXY(size_t x, size_t y, const Mesh* mesh);

    void RDPSimpPaths(Paths* paths);
    void RDPSimp(Path* path);
    void RDPSimpIter(Path* path, int start, int end, std::vector<bool>& keepingPointMrkArr);
    float distL2P(IntPoint* line_p1, IntPoint* line_p2, IntPoint* p);

    void offsetPath(Paths* paths);
    void offsetPaths_rec(Paths& paths);

    std::vector<XYArrangement> arng2D(std::vector<Paths>& figs);
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
    std::vector<Vecs> getObjVecsInRegions(Vecs& sub_vecs, Vecs& obj_vecs);
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

bool compareArea(std::pair<Paths*, int>& fig1, std::pair<Paths*, int>& fig2);


#endif // ARRANGE_H
