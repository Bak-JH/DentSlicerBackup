#include "VerticalSupportModel.h"
#include "SupportRaftManager.h"
#include "../DentEngine/src/configuration.h"
#include "../feature/Extrude.h"


constexpr  float SUPPORT_CONE_LENGTH =  1.0f;
constexpr  float SUPPORT_OVERLAP_LENGTH = SUPPORT_CONE_LENGTH/2;
constexpr  size_t VERT_SUPPORT_JOINT_CNT = 4;// extended tip, end cone(narrow end), start cone(wider end), base(support start)


using namespace Hix::Engine3D;
using namespace Hix::Input;
using namespace Hix::Render;
using namespace Hix::Support;
using namespace Hix::OverhangDetect;



Hix::Support::VerticalSupportModel::VerticalSupportModel(SupportRaftManager* manager, std::variant<VertexConstItr, FaceOverhang> overhang):
	SupportModel(manager, overhang)
{
	generateMesh();
	setMesh(_mesh);

}

VerticalSupportModel::~VerticalSupportModel()
{

}

QVector3D Hix::Support::VerticalSupportModel::getBasePt()
{
	return _basePt;
}

std::vector<QVector3D> Hix::Support::VerticalSupportModel::generateSupportPath(const std::variant<VertexConstItr, FaceOverhang>& overhang, float bottom)
{	 
	std::vector<QVector3D> path;
	
	//vertical support path should be very simple, cone pointy bit(endtip), cone ending bit, bottom bit.
	path.reserve(VERT_SUPPORT_JOINT_CNT);

	QVector3D coneNarrow;
	QVector3D tipNormal;
	//find the end tip of the support and normal at that point.
	if (overhang.index() == 0)
	{
		auto& vtx = std::get<0>(overhang);
		coneNarrow = vtx->position;
		tipNormal = vtx->vn;
	}
	else
	{
		auto& faceOverhang = std::get<1>(overhang);
		coneNarrow = faceOverhang.first;
		tipNormal = faceOverhang.second->fn;
	}
	//tip normal needs to be facing downard, ie) cone needs to be pointing upward,
	constexpr float normalizedVectorZMax = -0.44721f; //normalized vector 1, 0, 0.5
	tipNormal.normalize();
	tipNormal.setZ(std::min(tipNormal.z(), normalizedVectorZMax));

	//because of float error, it's safer to overlap support and mesh a little bit, so extend endtip into mesh a bit
	QVector3D extendedTip = coneNarrow;
	QVector3D intoMesh = -1.0f * tipNormal;
	intoMesh.normalize();
	intoMesh *= SUPPORT_OVERLAP_LENGTH;
	extendedTip += intoMesh;

	//cone of support is always in the direction of the overhang normal to minimize contact between support and mesh
	QVector3D coneWidePart = coneNarrow;
	coneWidePart += tipNormal * SUPPORT_CONE_LENGTH;
	//part of support that's in raft
	QVector3D supportStart = coneWidePart;
	supportStart.setZ(bottom);

	path.emplace_back(supportStart);
	path.emplace_back(coneWidePart);
	path.emplace_back(coneNarrow);
	path.emplace_back(extendedTip);
	_basePt = supportStart;
	return path;
}	 
	 

	 
void hexagonToTri(Mesh* supportMesh, const std::vector<QVector3D>& endCapOutline, const QVector3D& center, bool oppDir)
{	 
	if (oppDir)
	{
		for (size_t i = 0; i < 6; ++i)
		{
			supportMesh->addFace(center, endCapOutline[(i + 1) % 6], endCapOutline[i % 6]);
		}
	}
	else
	{
		for (size_t i = 0; i < 6; ++i)
		{
			supportMesh->addFace(center, endCapOutline[i % 6], endCapOutline[(i + 1) % 6]);
		}
	}

}	 


QVector3D Hix::Support::VerticalSupportModel::getPrimitiveColorCode(const Hix::Engine3D::Mesh* mesh, FaceConstItr faceItr)
{
	return QVector3D();
}

void Hix::Support::VerticalSupportModel::generateMesh()
{
	std::vector<QVector3D> contour;
	std::vector<QVector3D> path;
	std::vector<float> scales;

	path = generateSupportPath(_overhang, _manager->supportBottom());
	contour = generateHexagon(scfg->support_radius_max);

	//set scales
	float coneTipRadiusScale = scfg->support_radius_min/scfg->support_radius_max;
	scales.reserve(VERT_SUPPORT_JOINT_CNT);
	scales.emplace_back(1.0f);
	scales.emplace_back(1.0f);
	scales.emplace_back(coneTipRadiusScale);
	scales.emplace_back(coneTipRadiusScale);


	std::vector<std::vector<QVector3D>> jointContours;
	Hix::Features::Extrusion::extrudeAlongPath(_mesh, QVector3D(0,0,1), contour, path, jointContours, &scales);

	//create endcaps using joint contours;
	hexagonToTri(_mesh, jointContours.front(), path.front(), true);
	hexagonToTri(_mesh, jointContours.back(), path.back(), false);
}

std::vector<QVector3D> Hix::Support::generateHexagon(float radius)
{
	std::vector<QVector3D>hexagon;
	hexagon.reserve(6);

	constexpr  QVector3D normal(0, 0, 1);
	auto rot = QQuaternion::fromAxisAndAngle(normal, 60);
	auto pt = QVector3D(radius, 0, 0);

	for (size_t i = 0; i < 6; ++i)
	{

		hexagon.emplace_back(pt);
		pt = rot.rotatedVector(pt);
	}
	return hexagon;
}
