﻿#include "CylindricalRaft.h"
#include "SupportRaftManager.h"
#include "../DentEngine/src/configuration.h"
#include "../feature/Extrude.h"
#include "../feature/Shapes2D.h"
#include <QTransform>
#include <pcl/point_types.h>
#include <pcl/surface/concave_hull.h>
#include <pcl/surface/convex_hull.h>
#include <pcl/point_types.h>
#include "../render/LineMeshEntity.h"

constexpr float RAFT_JOINT_CNT = 2;

using namespace Hix::Engine3D;
using namespace Hix::Input;
using namespace Hix::Render;
using namespace Hix::Support;
using namespace Hix::OverhangDetect;



Hix::Support::CylindricalRaft::CylindricalRaft(SupportRaftManager* manager, const std::vector<QVector3D> basePt): RaftModel(manager)
{
	generateMesh(basePt);
}

Hix::Support::CylindricalRaft::~CylindricalRaft()
{
}

QVector3D Hix::Support::CylindricalRaft::getPrimitiveColorCode(const Hix::Engine3D::Mesh* mesh, FaceConstItr faceItr)
{
	throw std::runtime_error("Hix::Support::CylindricalRaft::getPrimitiveColorCode not implemented");
	return QVector3D();
}


void Hix::Support::CylindricalRaft::generateMeshForContour(Mesh* mesh, const std::vector<QVector3D>& contour)
{


	//need to form a contour that contains all overhangs
	std::vector<QVector3D> path;
	std::vector<float> scales;


	//path length is long, tips on both ends, one mid joint, 
	scales.reserve(RAFT_JOINT_CNT);
	path.reserve(RAFT_JOINT_CNT);

	//cylinder is of uniform shape, except it narrows a bit in the middle-z

	scales.emplace_back(0.4f);
	//scales.emplace_back(scale);
	scales.emplace_back(1.2f);

	//path is simple cylinder starting from 0,0,0 to 0,0,raft_height
	path.emplace_back(QVector3D(0, 0, _manager->raftBottom()));
	path.emplace_back(QVector3D(0, 0, scfg->raft_thickness));

	//create cylinder walls
	std::vector<std::vector<QVector3D>> jointContours;
	std::function<void(std::vector<QVector3D>&, float)> thicknessScaler(Hix::Shapes2D::scaleContourVtxNormal);
	Hix::Features::Extrusion::extrudeAlongPath(
		mesh, QVector3D(0, 0, 1), contour, path, jointContours, &scales, &thicknessScaler);
	//generate caps
	generateCap(mesh, jointContours.front(), true);
	generateCap(mesh, jointContours.back(), false);


}


void Hix::Support::CylindricalRaft::generateMesh(const std::vector<QVector3D>& overhangs)
{
	auto mesh = new Mesh();
	//generate square for each overhang
	//std::vector<std::vector<QVector3D>> cylinders;
	auto square = Hix::Shapes2D::generateSquare(scfg->support_radius_max*1.5);

	std::vector<QVector3D> contourPoints;
	contourPoints.reserve(4 * overhangs.size());
	//smaller bottom raft
	for (auto& overhang : overhangs)
	{
		auto curr = square;
		Hix::Shapes2D::moveContour(curr, overhang);
		std::move(std::begin(curr), std::end(curr), std::back_inserter(contourPoints));
		curr.clear();//remove zombies
	}
	
	//run concave hull algorithm on remaining points
	//set minimum voronoid distance to avoid crazy concave shapes
	constexpr float minVorDist = 4.0f;
	pcl::PointCloud<pcl::PointXYZ>::Ptr concaveOut(new pcl::PointCloud<pcl::PointXYZ>);
	pcl::PointCloud<pcl::PointXYZ>::Ptr interiorPts( new pcl::PointCloud<pcl::PointXYZ>);
	std::vector< pcl::Vertices > polygons;
	interiorPts->reserve(contourPoints.size());
	for (const auto& each : contourPoints)
	{
		interiorPts->push_back({ each.x(), each.y(), 0 });
	}
	pcl::ConcaveHull<pcl::PointXYZ> cHull;
	cHull.setAlpha(minVorDist);
	cHull.setInputCloud(interiorPts);
	auto test = cHull.getDimension();
	cHull.reconstruct(*concaveOut, polygons);
	for (auto& polygon : polygons)
	{
		std::vector<QVector3D> polygonVts;
		polygonVts.reserve(polygon.vertices.size());
		for (auto vtxIdx = polygon.vertices.cbegin(); vtxIdx != polygon.vertices.cend(); ++vtxIdx)
		{
			auto& vtx = (*concaveOut)[*vtxIdx];
			polygonVts.emplace_back(QVector3D(vtx.x, vtx.y, 0));
		}
		Hix::Shapes2D::ensureOrientation(false,polygonVts);
		generateMeshForContour(mesh, polygonVts);

	}


	setMesh(mesh);
	////generate hexagon contour for each overhang

}
