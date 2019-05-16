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
#if  defined(QT_DEBUG) || defined(_DEBUG)
#define _DEBUG_AUTO_ARRANGE
#endif

using namespace ClipperLib;

typedef std::pair<IntPoint, float> XYArrangement;
typedef std::vector<IntPoint> Vecs;

class autoarrange : public QObject {
    Q_OBJECT
public:
    autoarrange();

    std::vector<XYArrangement> simpArngMeshes(std::vector<const Mesh*> meshes);
    std::vector<XYArrangement> arngMeshes(std::vector<Mesh> meshes, std::vector<Qt3DCore::QTransform*> m_transform_set);
    void arrangeQt3D(std::vector<Qt3DCore::QTransform*> m_transform_set, std::vector<XYArrangement> arng_result_set);
    //void arrangeGlmodels(vector< GLModel* > * glmodels);

private:
    Paths getMeshRecArea(const Mesh& mesh);
    Paths getMeshConvexHull(const Mesh& mesh);
    Paths spreadingCheck(const Mesh* mesh, std::map<const MeshFace*, bool>& check, const MeshFace* chking_start, bool is_chking_pos);
	const MeshVertex* getPathHead(const Mesh* mesh, const MeshFace* mf, size_t side, bool is_chking_pos);
    Path buildOutline(const Mesh* mesh, std::map<const MeshFace*, bool>& check, const MeshFace* chking, const MeshVertex* path_head, bool is_chking_pos);
    bool isEdgeBound(const Mesh* mesh, const MeshFace* mf, size_t side, bool is_chking_pos);
    bool isNbrOrientSame(const Mesh* mesh, const MeshFace* mf, size_t side);
	size_t searchVtxInFace(const MeshFace* mf, const MeshVertex* vertex);
	const MeshVertex* getNbrVtx(const MeshFace* mf, size_t base, size_t xth);
    Path idxsToPath(const Mesh* mesh, std::vector<const MeshVertex* > path_by_idx);
    Paths project(const Mesh* mesh);
    Paths clipOutlines(std::vector<Paths> outline_sets);
    bool checkFNZ(const MeshFace* face, bool is_chking_pos);

	const MeshVertex* findVertexWithIntpoint(IntPoint p, const Mesh* mesh);
	const MeshVertex* findVertexWithIntXY(size_t x, size_t y, const Mesh* mesh);

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

    //void arrangeSingleQt3D(Qt3DCore::QTransform* m_transform, XYArrangement arng_result);



#ifdef  _DEBUG_AUTO_ARRANGE
	bool checkConvex(Path& path);
	void testSimplifyPolygon();
	void testClip();
	void testOffset();
	void debugPaths(Paths paths);
	void debugPath(Path path);
	void debugFaces(const Mesh* mesh, std::vector<const MeshFace*> face_list);
	void debugFace(const Mesh* mesh, const MeshFace* face);
#endif

};

bool compareArea(std::pair<Paths*, int>& fig1, std::pair<Paths*, int>& fig2);


#endif // ARRANGE_H
