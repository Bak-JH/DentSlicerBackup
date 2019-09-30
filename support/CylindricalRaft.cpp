#include "CylindricalRaft.h"
#include "SupportRaftManager.h"
#include "../DentEngine/src/configuration.h"
#include "../feature/Extrude.h"
#include "../feature/Shapes2D.h"

constexpr float RAFT_JOINT_CNT = 3;

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


void Hix::Support::CylindricalRaft::generateMeshForContour(Mesh* mesh, const ClipperLib::Path& contourPath)
{


	//need to form a contour that contains all overhangs
	std::vector<QVector3D> contour;
	std::vector<QVector3D> path;
	std::vector<float> scales;


	//path length is long, tips on both ends, one mid joint, 
	scales.reserve(RAFT_JOINT_CNT);
	path.reserve(RAFT_JOINT_CNT);

	//cylinder is of uniform shape, except it narrows a bit in the middle-z
	constexpr float scale = 0.9f;

	scales.emplace_back(1.0f);
	scales.emplace_back(scale);
	scales.emplace_back(1.0f);

	//path is simple cylinder starting from 0,0,0 to 0,0,raft_height
	path.emplace_back(QVector3D(0, 0, _manager->raftBottom()));
	path.emplace_back(QVector3D(0, 0, _manager->raftBottom() + scfg->raft_thickness / 2.0f));
	path.emplace_back(QVector3D(0, 0, _manager->raftBottom() + scfg->raft_thickness));

	//convert to float point contour
	contour.reserve(contourPath.size());
	for (auto& each : contourPath)
	{
		contour.emplace_back(Hix::Polyclipping::toFloatPt(each));
	}

	//TODO get rid of this ugly messs
	float xCenter = 0;
	float yCenter = 0;
	for (auto& pt : contour)
	{
		xCenter += pt.x();
		yCenter += pt.y();
	}
	xCenter /= contour.size();
	yCenter /= contour.size();
	QVector2D centoid(xCenter, yCenter);


	//create cylinder walls
	std::vector<std::vector<QVector3D>> jointContours;
	Hix::Features::Extrusion::extrudeAlongPath(mesh, QVector3D(0, 0, 1), contour, path, jointContours, &scales, &centoid);
	//generate caps
	generateCap(mesh, jointContours.front(), true);
	generateCap(mesh, jointContours.back(), false);


}

void Hix::Support::CylindricalRaft::generateCap(Mesh* mesh, const std::vector<QVector3D>& contour, bool isReverse)
{
	float z = contour.front().z();
	std::vector<p2t::Point>container;
	container.reserve(contour.size());
	for (auto itr = contour.cbegin(); itr != contour.cend(); ++itr)
	{
		container.emplace_back((double)itr->x(), (double)itr->y());
	}


	//because people are...stupid?
	std::vector<p2t::Point*>ptrs;
	ptrs.reserve(container.size());
	for (auto& each : container)
	{
		ptrs.emplace_back(&each);
	}

	p2t::CDT constrainedDelTrig(ptrs);
	constrainedDelTrig.Triangulate();
	auto triangles = constrainedDelTrig.GetTriangles();

	//as long as intial contour is not modified in the p2t library, float->double->float should be lossless
	for (auto& tri : triangles)
	{
		p2t::Point* pt0, *pt1, *pt2;
		if (isReverse)
		{
			pt2 = tri->GetPoint(0);
			pt1 = tri->GetPoint(1);
			pt0 = tri->GetPoint(2);
		}
		else
		{
			pt0 = tri->GetPoint(0);
			pt1 = tri->GetPoint(1);
			pt2 = tri->GetPoint(2);
		}
		mesh->addFace(
			QVector3D(pt0->x, pt0->y, z),
			QVector3D(pt1->x, pt1->y, z),
			QVector3D(pt2->x, pt2->y, z));
	}

}

void Hix::Support::CylindricalRaft::generateMesh(const std::vector<QVector3D>& overhangs)
{
	auto mesh = new Mesh();

	//need to form a contour that contains all overhangs
	std::vector<std::vector<QVector3D>> cylinders;
	auto hexagon = generateHexagon(scfg->raft_base_radius());

	//generate hexagon contour for each overhang
	cylinders.reserve(overhangs.size());
	for (auto& overhang : overhangs)
	{
		auto currHexagon = hexagon;
		Hix::Shapes2D::moveContour(currHexagon, overhang);
		cylinders.emplace_back(std::move(currHexagon));
	}
	//combine area of all hexagons into a single shape;
	auto contours = Hix::Shapes2D::combineContour(cylinders);
	for (auto& contour : contours)
	{
		generateMeshForContour(mesh, contour);
	}
	setMesh(mesh);
}
