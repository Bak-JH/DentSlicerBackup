#include "CrossSectionPlane.h"
#include "../../application/ApplicationManager.h"
#include "../../slice/slicingengine.h"
#include "../Shapes2D.h"
#include "../../glmodel.h"
using namespace Hix::Input;
using namespace Hix::Features;
using namespace Hix::Render;
using namespace Hix::Slicer;
using namespace Hix::Polyclipping;


Hix::Features::CrossSectionPlane::CrossSectionPlane(Qt3DCore::QEntity* owner): SceneEntityWithMaterial(owner)
{
}

void Hix::Features::CrossSectionPlane::init(const std::unordered_set<GLModel*>& selectedModels)
{
	//slice
	auto selectedBound = Hix::Engine3D::combineBounds(selectedModels);
	auto shellSlices = SlicingEngine::sliceModels(selectedBound.zMax(), selectedModels, Hix::Application::ApplicationManager::getInstance().supportRaftManager());
	_layerMeshes.reserve(shellSlices.size());

	//for each polytree from slice, triangluate, create mesh
	for (auto& s : shellSlices)
	{
		_layerMeshes.emplace_back();
		auto& mesh = _layerMeshes.back();
		PolytreeCDT polycdt(&s.polytree);
		auto trigMap = polycdt.triangulate();

		//generate front & back mesh
		for (auto node : trigMap)
		{
			for (auto trig : node.second)
			{
				mesh.addFace(
					QVector3D(trig[0].x(), trig[0].y(), 0),
					QVector3D(trig[1].x(), trig[1].y(), 0),
					QVector3D(trig[2].x(), trig[2].y(), 0)
				);
			}
		}
	}
}

Hix::Features::CrossSectionPlane::~CrossSectionPlane()
{
}

void Hix::Features::CrossSectionPlane::showLayer(size_t layer)
{
	layer = std::min(_layerMeshes.size() - 1, layer);
	setMesh(&_layerMeshes[layer]);

	//set z
	float z = layer * Hix::Application::ApplicationManager::getInstance().settings().sliceSetting.layerHeight;
	//set to identity
	transform().setMatrix(QMatrix4x4());
	transform().setTranslation(QVector3D(0, 0, z));
}


QVector4D Hix::Features::CrossSectionPlane::getPrimitiveColorCode(const Hix::Engine3D::Mesh* mesh, FaceConstItr faceItr)
{
	throw std::runtime_error("Hix::Support::CylindricalRaft::getPrimitiveColorCode not implemented");
	return QVector4D();
}