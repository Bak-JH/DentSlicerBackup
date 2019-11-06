#include "modelcut.h"
#include <algorithm>
#include <queue>
#include <map>
#include <string.h>
#include <stdlib.h>
#include <QObject>
#include <Qt3DCore>
#include <Qt3DRender>
#include <Qt3DExtras>
#include <Qt3DInput>
#include <QEntity>
#include "../../glmodel.h"
#include "../../qmlmanager.h"
#include "feature/Shapes2D.h"
#include "feature/Extrude.h"
#include <vector>

#include "DentEngine/src/mesh.h"
#include "../repair/meshrepair.h"
#include "polylinecut.h"
#include "../CSG/CSG.h"
//#include "DentEngine/src/utils/metric.h"
//#include "feature/convex_hull.h"
//#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
//#include <CGAL/Constrained_Delaunay_triangulation_2.h>
//#include <CGAL/Delaunay_triangulation_2.h>
//#include <CGAL/Delaunay_mesh_vertex_base_2.h>
//#include <CGAL/Delaunay_mesh_face_base_2.h>
//#include <CGAL/Delaunay_mesh_size_criteria_2.h>
//#include <CGAL/Delaunay_mesher_2.h>
//#include <math.h>
//
//typedef CGAL::Exact_predicates_inexact_constructions_kernel  Kernel;
//typedef Kernel::Point_2            Point_2;
//typedef CGAL::Delaunay_mesh_vertex_base_2<Kernel>     Vb;
//typedef CGAL::Delaunay_mesh_face_base_2<Kernel>      Fb;
//typedef CGAL::Triangulation_data_structure_2<Vb, Fb>   Tds;
//typedef CGAL::Constrained_Delaunay_triangulation_2<Kernel, Tds>  CDT;
//typedef CDT::Vertex_handle          Vertex_handle;
//typedef CGAL::Delaunay_mesh_size_criteria_2<CDT>    Criteria;

using namespace Hix::Engine3D;
using namespace Hix::Slicer;
using namespace Hix::Features;
using namespace Hix::Features::CSG;
using namespace Hix::Features::Cut;


Hix::Features::Cut::PolylineCut::PolylineCut(GLModel * origModel, std::vector<QVector3D> _cuttingPoints)

{
	//convert polyline to CSG-able 3D mesh, a thin 3D wall.
	Mesh polylineWall;

	generateCuttingWalls(_cuttingPoints, origModel->recursiveAabb(), polylineWall);
	//convert all meshes to cork meshes
	auto cylinderWallCork = toCorkMesh(polylineWall);
	cutCSG(origModel->modelName(), origModel, cylinderWallCork);
	freeCorkTriMesh(&cylinderWallCork);

}

void Hix::Features::Cut::PolylineCut::generateCuttingWalls(const std::vector<QVector3D>& polyline, const Hix::Engine3D::Bounds3D& cutBound, Hix::Engine3D::Mesh& out)
{
	//to 2d polygon
	auto polyline2d = Hix::Shapes2D::to2DShape(polyline);
	auto contour2d = Hix::Shapes2D::PolylineToArea(0.001f, polyline2d);
	auto contour3d = Hix::Shapes2D::to3DShape(0.0f, contour2d);

	std::vector<QVector3D> path;
	path.reserve(2);
	path.emplace_back(QVector3D(0, 0, -0.5));
	path.emplace_back(QVector3D(0, 0, cutBound.zMax() + 0.5));
	std::vector<std::vector<QVector3D>> jointContours;
	Hix::Features::Extrusion::extrudeAlongPath<float>(&out, QVector3D(0, 0, 1), contour3d, path, jointContours);
	//generate caps
	Hix::Shapes2D::generateCapZPlane(&out, jointContours.front(), true);
	Hix::Shapes2D::generateCapZPlane(&out, jointContours.back(), false);
}

void Hix::Features::Cut::PolylineCut::cutCSG(const QString& subjectName, Hix::Render::SceneEntity* subject, const CorkTriMesh& subtract)
{
	size_t childIdx = 0;
	for (auto childNode : subject->childNodes())
	{
		auto entity = dynamic_cast<Hix::Render::SceneEntity*>(childNode);
		if (entity)
		{
			auto childName = subjectName + "_child" + QString::number(childIdx);
			cutCSG(childName, entity, subtract);
			++childIdx;
		}
	}

	auto subjectCork = toCorkMesh(*subject->getMesh());
	CorkTriMesh output;
	computeDifference(subjectCork, subtract, &output);

	//convert the result back to hix mesh
	auto result = toHixMesh(output);
	//manual free cork memory, TODO RAII
	freeCorkTriMesh(&subjectCork);
	freeCorkTriMesh(&output);

	//seperate disconnected meshes
	auto seperateParts = Hix::Features::seperateDisconnectedMeshes(result);
	for (size_t i = 0; i < seperateParts.size(); ++i)
	{
		auto model = qmlManager->createAndListModel(seperateParts[i], subjectName + "_cut" + QString::number(i), &subject->transform());
	}
	subject->setMesh(nullptr);
	auto model = dynamic_cast<GLModel*>(subject);
	if (model)
	{
		qmlManager->deleteModelFile(model->ID);
	}
	else
	{
		delete subject;
	}

}

