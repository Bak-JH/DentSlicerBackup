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

#include "Mesh/mesh.h"
#include "../repair/meshrepair.h"
#include "polylinecut.h"
#include "../CSG/CSG.h"
#include "../cdt/HixCDT.h"
#include "../../common/Debug.h"

using namespace Hix::Slicer;
using namespace Hix::Features;
using namespace Hix::Features::CSG;
using namespace Hix::Features::Cut;


Hix::Features::Cut::PolylineCut::PolylineCut(GLModel* target, std::vector<QVector3D> cuttingPoints)
	: FeatureContainerFlushSupport(target), _cuttingPoints(cuttingPoints)
{
	_targets.insert(target);
}

Hix::Features::Cut::PolylineCut::PolylineCut(std::unordered_set<GLModel*> targets, std::vector<QVector3D> cuttingPoints)
	: FeatureContainerFlushSupport(targets), _targets(targets), _cuttingPoints(cuttingPoints)
{
}

void Hix::Features::Cut::PolylineCut::runImpl()
{
	//convert polyline to CSG-able 3D mesh, a thin 3D wall.
	for (auto each : _targets)
	{
		Hix::Engine3D::Mesh polylineWall;

		generateCuttingWalls(_cuttingPoints, each->recursiveAabb(), polylineWall);
		//convert all meshes to cork meshes
		auto cylinderWallCork = toCorkMesh(polylineWall);
		cutCSG(each->modelName(), each, cylinderWallCork);
		freeCorkTriMesh(&cylinderWallCork);
	}
}

void Hix::Features::Cut::PolylineCut::generateCuttingWalls(const std::vector<QVector3D>& polyline, const Hix::Engine3D::Bounds3D& cutBound, Hix::Engine3D::Mesh& out)
{
	//to 2d polygon
	auto polyline2d = Hix::Shapes2D::to2DShape(polyline);
	auto contour2d = Hix::Shapes2D::PolylineToArea(0.001f, polyline2d);
	auto contour3d = Hix::Shapes2D::to3DShape(0.0f, contour2d);
	//postUIthread([this, contour3d]()
	//	{
	//		Hix::Debug::DebugRenderObject::getInstance().addPath(contour3d);
	//	});


	std::vector<QVector3D> path;
	path.reserve(2);
	path.emplace_back(QVector3D(0, 0, cutBound.zMin() -0.5));
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

void Hix::Features::Cut::PolylineCut::cutCSG(const QString& targetName, Hix::Render::SceneEntity* target, const CorkTriMesh& subtract)
{
	size_t childIdx = 0;
	for (auto childNode : target->childNodes())
	{
		auto entity = dynamic_cast<Hix::Render::SceneEntity*>(childNode);
		if (entity)
		{
			auto childName = targetName + "_child" + QString::number(childIdx);
			cutCSG(childName, entity, subtract);
			++childIdx;
		}
	}

	auto targetCork = toCorkMesh(*target->getMesh());
	CorkTriMesh output;
	computeDifference(targetCork, subtract, &output);

	//convert the result back to hix mesh
	auto result = toHixMesh(output);
	//manual free cork memory, TODO RAII
	freeCorkTriMesh(&targetCork);
	freeCorkTriMesh(&output);

	//seperate disconnected meshes
	auto seperateParts = Hix::Features::seperateDisconnectedMeshes(result);
	for (size_t i = 0; i < seperateParts.size(); ++i)
	{
		if (seperateParts[i]->getFaces().size() < 3)
			continue;

		float filter = float(seperateParts[i]->getFaces().size()) / float(target->getMesh()->getFaces().size());
		if (filter < 0.0001f)
		{
			continue;
		}

		auto addModel = new Hix::Features::ListModel(seperateParts[i], targetName + "_cut" + QString::number(i), nullptr);
		tryRunFeature(*addModel);
		addModel->get()->setZToBed();
		addFeature(addModel);
	}
	auto model = dynamic_cast<GLModel*>(target);
	if (model)
	{
		auto deleteModel = new DeleteModel(model);
		tryRunFeature(*deleteModel);
		addFeature(deleteModel);
	}
	else
	{
		delete target;
	}
}

