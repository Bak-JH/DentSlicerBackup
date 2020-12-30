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

#include "feature/Shapes2D.h"
#include "feature/Extrude.h"
#include <vector>
#include "feature/addModel.h"
#include "feature/deleteModel.h"

#include "DentEngine/src/mesh.h"
#include "../repair/meshrepair.h"
#include "polylinecut.h"
#include "../CSG/CSG.h"
#include "../cdt/HixCDT.h"

using namespace Hix::Engine3D;
using namespace Hix::Slicer;
using namespace Hix::Features;
using namespace Hix::Features::CSG;
using namespace Hix::Features::Cut;


Hix::Features::Cut::PolylineCut::PolylineCut(GLModel * origModel, std::vector<QVector3D> cuttingPoints): FeatureContainerFlushSupport(origModel), _target(origModel), _cuttingPoints(cuttingPoints)
{
}

void Hix::Features::Cut::PolylineCut::runImpl()
{
	//convert polyline to CSG-able 3D mesh, a thin 3D wall.
	Mesh polylineWall;

	generateCuttingWalls(_cuttingPoints, _target->recursiveAabb(), polylineWall);
	//convert all meshes to cork meshes
	auto cylinderWallCork = toCorkMesh(polylineWall);
	cutCSG(_target->modelName(), _target, cylinderWallCork);
	freeCorkTriMesh(&cylinderWallCork);

	if (isRepairNeeded(_target->getMesh()))
	{
		auto repair = new MeshRepair(_target);
		tryRunFeature(*repair);
	}
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
	auto jointDir = Hix::Features::Extrusion::interpolatedJointNormals(path);
	auto jointContours = Hix::Features::Extrusion::extrudeAlongPath(&out, QVector3D(0, 0, 1), contour3d, path, jointDir);
	//generate caps
	if (jointContours.front().size() > 1)
	{
		Hix::CDT::MeshCDT cdt(&out, true);
		cdt.insertSolidContourZAxis(jointContours.front());
		cdt.triangulateAndAppend();
	}

	if (jointContours.back().size() > 1)
	{
		Hix::CDT::MeshCDT cdt(&out, false);
		cdt.insertSolidContourZAxis(jointContours.back());
		cdt.triangulateAndAppend();
	}
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
		if (seperateParts[i]->getFaces().empty())
			continue;
		auto addModel = new Hix::Features::ListModel(seperateParts[i], subjectName + "_cut" + QString::number(i), nullptr);
		tryRunFeature(*addModel);
		addModel->get()->setZToBed();
		addFeature(addModel);
	}
	auto model = dynamic_cast<GLModel*>(subject);
	if (model)
	{
		auto deleteModel = new DeleteModel(model);
		tryRunFeature(*deleteModel);
		addFeature(deleteModel);
	}
	else
	{
		delete subject;
	}
}

