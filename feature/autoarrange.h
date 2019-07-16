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
#include "DentEngine/src/polyclipping/clipper/clipper.hpp"
#include "convex_hull.h"
#if  defined(QT_DEBUG) || defined(_DEBUG)
#define _DEBUG_AUTO_ARRANGE
#endif
namespace Hix
{
	namespace Engine3D
	{
		class Mesh;
		struct HalfEdge;
		struct MeshVertex;
		struct MeshFace;

	}
}


typedef std::pair<ClipperLib::IntPoint, float> XYArrangement;
typedef std::vector<ClipperLib::IntPoint> Vecs;

class autoarrange : public QObject {
    Q_OBJECT
public:
    autoarrange();

    std::vector<XYArrangement> simpArngMeshes(std::vector<const Hix::Engine3D::Mesh*>& meshes);
    std::vector<XYArrangement> arngMeshes(std::vector<const Hix::Engine3D::Mesh*>& meshes, std::vector<const Qt3DCore::QTransform*> m_transform_set);
    //void arrangeQt3D(std::vector<const Qt3DCore::QTransform*> m_transform_set, std::vector<XYArrangement> arng_result_set);
    //void arrangeGlmodels(vector< GLModel* > * glmodels);

private:
    ClipperLib::Paths getMeshRecArea(const Hix::Engine3D::Mesh& mesh);
    ClipperLib::Paths getMeshConvexHull(const Hix::Engine3D::Mesh& mesh);
    ClipperLib::Paths spreadingCheck(const Hix::Engine3D::Mesh* mesh, std::map<const Hix::Engine3D::MeshFace *, bool>& check, const Hix::Engine3D::MeshFace * chking_start, bool is_chking_pos);
    ClipperLib::Path buildOutline(const Hix::Engine3D::Mesh* mesh, std::map<const Hix::Engine3D::MeshFace *, bool>& check, const Hix::Engine3D::MeshFace * chking, const Hix::Engine3D::MeshVertex* path_head, bool is_chking_pos);
    bool isEdgeBound(const Hix::Engine3D::Mesh* mesh, const Hix::Engine3D::MeshFace * mf, const Hix::Engine3D::HalfEdge * edge, bool is_chking_pos);
    bool isNbrOrientSame(const Hix::Engine3D::Mesh* mesh, const Hix::Engine3D::MeshFace * mf, const Hix::Engine3D::HalfEdge * edge);
	//size_t searchVtxInFace(const Hix::Engine3D::MeshFace * mf, const Hix::Engine3D::MeshVertex* vertex);
	const Hix::Engine3D::MeshVertex* getNbrVtx(const Hix::Engine3D::MeshFace * mf, const Hix::Engine3D::HalfEdge * base, size_t offset);
    ClipperLib::Path idxsToPath(const Hix::Engine3D::Mesh* mesh, std::vector<const Hix::Engine3D::MeshVertex* > path_by_idx);
    ClipperLib::Paths project(const Hix::Engine3D::Mesh* mesh);
    ClipperLib::Paths clipOutlines(std::vector<ClipperLib::Paths> outline_sets);
    bool checkFNZ(const Hix::Engine3D::MeshFace& face, bool is_chking_pos);

	const Hix::Engine3D::MeshVertex* findVertexWithIntpoint(ClipperLib::IntPoint p, const Hix::Engine3D::Mesh* mesh);
	const Hix::Engine3D::MeshVertex* findVertexWithIntXY(size_t x, size_t y, const Hix::Engine3D::Mesh* mesh);

    void RDPSimpPaths(ClipperLib::Paths* paths);
    void RDPSimp(ClipperLib::Path* path);
    void RDPSimpIter(ClipperLib::Path* path, int start, int end, std::vector<bool>& keepingPointMrkArr);
    float distL2P(ClipperLib::IntPoint* line_p1, ClipperLib::IntPoint* line_p2, ClipperLib::IntPoint* p);

    void offsetPath(ClipperLib::Paths* paths);
    void offsetPaths_rec(ClipperLib::Paths& paths);

    std::vector<XYArrangement> arng2D(std::vector<ClipperLib::Paths>& figs);
    void initStage(ClipperLib::Paths& cum_outline);
    XYArrangement newArngFig(ClipperLib::Paths& cum_outline, ClipperLib::Paths& fig);
    XYArrangement arngFig(ClipperLib::Paths& cum_outline, ClipperLib::Paths& fig);
    ClipperLib::Paths rotatePaths(ClipperLib::Paths& paths, float rot_angle);
    ClipperLib::IntPoint rotatePoint(ClipperLib::IntPoint& point, float rot_angle);
    void tanslatePaths(ClipperLib::Paths& paths, ClipperLib::IntPoint& tanslate_vec);
    ClipperLib::IntPoint tanslatePoint(ClipperLib::IntPoint& point, ClipperLib::IntPoint& tanslate_vec);
    void cumulativeClip(ClipperLib::Paths& cum_outline, ClipperLib::Paths& new_fig);
    ClipperLib::IntPoint getOptimalPoint(ClipperLib::Paths& nfp_set);
    int getMaxX(ClipperLib::Path& path);
    int getMinX(ClipperLib::Path& path);
    int getMaxY(ClipperLib::Path& path);
    int getMinY(ClipperLib::Path& path);
    ClipperLib::Paths getNFP(ClipperLib::Paths& subject, ClipperLib::Paths& object);
    ClipperLib::Paths simplyfyRawNFP(ClipperLib::Paths& raw_nfp_set, ClipperLib::Paths& subject);
    ClipperLib::Paths mergeNFP(ClipperLib::Paths& separate_nfp_set);
    std::vector<Vecs> getObjVecsInRegions(Vecs& sub_vecs, Vecs& obj_vecs);
    bool isVecAlign(const ClipperLib::IntPoint& a, const ClipperLib::IntPoint& b);
    bool isCCW(const ClipperLib::IntPoint& a, const ClipperLib::IntPoint& b, const ClipperLib::IntPoint& c);
    bool isVecCCW(const ClipperLib::IntPoint& a, const ClipperLib::IntPoint& b, const ClipperLib::IntPoint& c);
    ClipperLib::IntPoint getEdgeVec(const ClipperLib::Path& path, int edge_idx, bool isForward);
    ClipperLib::IntPoint getFirstNFPPoint(const ClipperLib::IntPoint& first_sub_vec, const ClipperLib::IntPoint& first_sub_vtx, const ClipperLib::Path& obj, const Vecs& obj_vecs);

    //void arrangeSingleQt3D(Qt3DCore::QTransform* m_transform, XYArrangement arng_result);



#ifdef  _DEBUG_AUTO_ARRANGE
	bool checkConvex(ClipperLib::Path& path);
	void testSimplifyPolygon();
	void testClip();
	void testOffset();
	void debugPaths(ClipperLib::Paths paths);
	void debugPath(ClipperLib::Path path);
	void debugFaces(const Hix::Engine3D::Mesh* mesh, std::vector<const Hix::Engine3D::MeshFace *> face_list);
	void debugFace(const Hix::Engine3D::Mesh* mesh, const Hix::Engine3D::MeshFace * face);
#endif

};

bool compareArea(std::pair<ClipperLib::Paths*, int>& fig1, std::pair<ClipperLib::Paths*, int>& fig2);


#endif // ARRANGE_H
