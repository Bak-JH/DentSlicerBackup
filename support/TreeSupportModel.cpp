#include "TreeSupportModel.h"
#include "SupportRaftManager.h"
#include "../DentEngine/src/configuration.h"
#include "../feature/Extrude.h"
#include "TreeSupportGenerate.h"


constexpr  float SUPPORT_CONE_LENGTH = 1.0f;
constexpr  float SUPPORT_OVERLAP_LENGTH = SUPPORT_CONE_LENGTH / 2;
constexpr  size_t VERT_SUPPORT_JOINT_CNT = 4;// extended tip, end cone(narrow end), start cone(wider end), base(support start)


using namespace Hix::Engine3D;
using namespace Hix::Input;
using namespace Hix::Render;
using namespace Hix::Support;
using namespace Hix::OverhangDetect;


Hix::Support::TreeSupportModel::TreeSupportModel(SupportRaftManager* manager, TreeSupportGenerate* tree_manager, std::variant<VertexConstItr, FaceOverhang> overhang,
	QVector3D top, QVector3D bottom) :
	SupportModel(manager, overhang)
{
	_tree_manager = tree_manager;
	generateMesh(top, bottom);
	setMesh(_mesh);

}

TreeSupportModel::~TreeSupportModel()
{

}

QVector3D Hix::Support::TreeSupportModel::getBasePt()
{
	return _basePt;
}

std::vector<QVector3D> Hix::Support::TreeSupportModel::generateSupportPath(const QVector3D top, const QVector3D bottom)
{
	std::vector<QVector3D> path;

	//vertical support path should be very simple, cone pointy bit(endtip), cone ending bit, bottom bit.
	path.reserve(VERT_SUPPORT_JOINT_CNT);

	QVector3D coneNarrow = top;
	QVector3D tipNormal = _tree_manager->tree_support_info[top].normal;

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
	supportStart.setZ(bottom.z());

	path.emplace_back(supportStart);
	path.emplace_back(coneWidePart);
	path.emplace_back(coneNarrow);
	path.emplace_back(extendedTip);
	_basePt = supportStart;
	return path;
}

void hexagonToTri2(Mesh* supportMesh, const std::vector<QVector3D>& endCapOutline, const QVector3D& center, bool oppDir)
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


QVector3D Hix::Support::TreeSupportModel::getPrimitiveColorCode(const Hix::Engine3D::Mesh* mesh, FaceConstItr faceItr)
{
	return QVector3D();
}

void Hix::Support::TreeSupportModel::generateMesh(QVector3D top, QVector3D bottom)
{
	std::vector<QVector3D> contour;
	std::vector<QVector3D> path;
	std::vector<float> scales;

	path = generateSupportPath(top, bottom);
	contour = generateHexagon(scfg->support_radius_max);

	//set scales
	float coneTipRadiusScale = scfg->support_radius_min / scfg->support_radius_max;
	scales.reserve(VERT_SUPPORT_JOINT_CNT);
	scales.emplace_back(1.0f);
	scales.emplace_back(1.0f);
	scales.emplace_back(coneTipRadiusScale);
	scales.emplace_back(coneTipRadiusScale);


	std::vector<std::vector<QVector3D>> jointContours;
	Hix::Features::Extrusion::extrudeAlongPath(_mesh, QVector3D(0, 0, 1), contour, path, jointContours, &scales);

	//create endcaps using joint contours;
	hexagonToTri2(_mesh, jointContours.front(), path.front(), true);
	hexagonToTri2(_mesh, jointContours.back(), path.back(), false);
}

