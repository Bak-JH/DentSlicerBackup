#include "VerticalSupportModel.h"
#include "SupportRaftManager.h"
#include "../DentEngine/src/configuration.h"
#include "../feature/Extrude.h"
#include "SupportRaftManager.h"
#include "../glmodel.h"

constexpr  float SUPPORT_CONE_LENGTH =  1.0f;
constexpr  float SUPPORT_OVERLAP_LENGTH = SUPPORT_CONE_LENGTH/2;
constexpr  size_t VERT_SUPPORT_JOINT_CNT = 4;// extended tip, end cone(narrow end), start cone(wider end), base(support start)


using namespace Hix::Engine3D;
using namespace Hix::Input;
using namespace Hix::Render;
using namespace Hix::Support;
using namespace Hix::OverhangDetect;
using namespace Qt3DCore;



Hix::Support::VerticalSupportModel::VerticalSupportModel(SupportRaftManager* manager, const Overhang& overhang):
	SupportModel(manager), _overhang(overhang)
{
	generateMesh();

}

VerticalSupportModel::~VerticalSupportModel()
{
	//unlike other SceneEntities, Raft and support owns their mesh data
}

const QVector3D& Hix::Support::VerticalSupportModel::getBasePt() const
{
	return _basePt;
}

const Overhang& Hix::Support::VerticalSupportModel::getOverhang() const
{
	return _overhang;
}

std::vector<QVector3D> Hix::Support::VerticalSupportModel::generateSupportPath(float bottom)
{	 
	std::vector<QVector3D> path;
	
	//vertical support path should be very simple, cone pointy bit(endtip), cone ending bit, bottom bit.
	path.reserve(VERT_SUPPORT_JOINT_CNT);

	QVector3D coneNarrow;
	QVector3D tipNormal;
	//find the end tip of the support and normal at that point.
	if (_overhang.index() == 0)
	{
		auto& vtx = std::get<0>(_overhang);
		coneNarrow = vtx.worldPosition();
		tipNormal = vtx.worldVn();
	}
	else
	{
		auto& faceOverhang = std::get<1>(_overhang);
		coneNarrow = faceOverhang.coord;
		tipNormal = faceOverhang.face.worldFn();
	}
	//tip normal needs to be facing downard, ie) cone needs to be pointing upward,
	constexpr float normalizedVectorZMax = -1.0f; //tan 45
	QVector2D xy(tipNormal.x(), tipNormal.y());
	auto zMax = normalizedVectorZMax * xy.length();
	tipNormal.setZ(std::min(zMax, tipNormal.z()));
	tipNormal.normalize();

	//because of float error, it's safer to overlap support and mesh a little bit, so extend endtip into mesh a bit
	QVector3D extendedTip = coneNarrow;
	QVector3D intoMesh = -1.0f * tipNormal;
	intoMesh *= SUPPORT_OVERLAP_LENGTH;
	extendedTip += intoMesh;

	//cone of support is always in the direction of the overhang normal to minimize contact between support and mesh
	QVector3D coneWidePart = coneNarrow;
	coneWidePart += tipNormal * SUPPORT_CONE_LENGTH;
	//part of support that's in raft
	QVector3D supportStart = coneWidePart;

	supportStart.setZ(0);

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

	path = generateSupportPath(_manager->supportBottom());
	contour = generateHexagon(scfg->support_radius_max);

	//set scales
	float coneTipRadiusScale = scfg->support_radius_min/scfg->support_radius_max;
	scales.reserve(VERT_SUPPORT_JOINT_CNT);
	scales.emplace_back(1.0f);
	scales.emplace_back(1.0f);
	scales.emplace_back(coneTipRadiusScale);
	scales.emplace_back(coneTipRadiusScale);

	auto mesh = new Mesh();
	std::vector<std::vector<QVector3D>> jointContours;
	Hix::Features::Extrusion::extrudeAlongPath(mesh, QVector3D(0,0,1), contour, path, jointContours, &scales);

	//create endcaps using joint contours;
	hexagonToTri(mesh, jointContours.front(), path.front(), true);
	hexagonToTri(mesh, jointContours.back(), path.back(), false);
	setMesh(mesh);

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
