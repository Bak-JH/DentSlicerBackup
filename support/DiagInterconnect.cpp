﻿#include "DiagInterconnect.h"
#include "SupportRaftManager.h"
#include "../feature/Extrude.h"
#include "SupportRaftManager.h"
#include "../glmodel.h"
#include "../application/ApplicationManager.h"
constexpr  float SUPPORT_CONE_LENGTH =  1.0f;
//constexpr  float SUPPORT_OVERLAP_LENGTH = SUPPORT_CONE_LENGTH/2;

using namespace Hix::Shapes2D;
using namespace Hix::Engine3D;
using namespace Hix::Input;
using namespace Hix::Render;
using namespace Hix::Support;
using namespace Hix::OverhangDetect;
using namespace Qt3DCore;
using namespace Hix::Features::Extrusion;






Hix::Support::DiagInterconnect::DiagInterconnect(SupportRaftManager* manager, std::array<QVector3D, 2> pts, std::array<SupportModel*, 2> connectedModels): 
	SupportModel(manager), Interconnect(manager), _pts(pts)
{
	Interconnect::_connected = connectedModels;
	generateMesh();
}

Hix::Support::DiagInterconnect::~DiagInterconnect()
{
}

QVector4D Hix::Support::DiagInterconnect::getPrimitiveColorCode(const Hix::Engine3D::Mesh* mesh, Hix::Engine3D::FaceConstItr faceItr)
{
	return QVector4D();
}

void Hix::Support::DiagInterconnect::generateMesh()
{
	std::vector<QVector3D> contour;
	std::vector<QVector3D> crossContour;
	std::vector<float> scales;
	auto mesh = new Mesh();

	auto radiusMax = Hix::Application::ApplicationManager::getInstance().settings().supportSetting.supportRadiusMax;
	_contour.emplace_back(_pts[0]); //min
	_contour.emplace_back(_pts[1]); //min
	auto dir = _pts[1] - _pts[0];
	dir.normalize();
	if (!_contour.empty())
	{
		crossContour = generateCircle(radiusMax, 16);

		
		auto jointDir = Hix::Features::Extrusion::interpolatedJointNormals(_contour);
		auto jointContours = extrudeAlongPath(mesh, QVector3D(0, 0, 1), crossContour, _contour, jointDir);

		//create endcaps using joint contours;
		circleToTri(mesh, jointContours.front(), _contour.front(), true);
		circleToTri(mesh, jointContours.back(), _contour.back(), false);
	}
	setMesh(mesh);

}
