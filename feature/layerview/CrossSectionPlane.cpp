#include "CrossSectionPlane.h"
#include "../../application/ApplicationManager.h"
#include "../../slice/slicingengine.h"
#include "../Shapes2D.h"
#include "../../glmodel.h"
#include "../../DentEngine/src/SlicerDebug.h"
#include "../../DentEngine/src/polyclipping/polytree2tri/PolytreeCDT.h"

using namespace Hix::Input;
using namespace Hix::Features;
using namespace Hix::Render;
using namespace Hix::Slicer;
using namespace Hix::Polyclipping;


Hix::Features::CrossSectionPlane::CrossSectionPlane(Qt3DCore::QEntity* owner): SceneEntityWithMaterial(owner)
{
	setMaterialColor(Hix::Render::Colors::Selected);

}

void Hix::Features::CrossSectionPlane::init(const std::unordered_map<SceneEntityWithMaterial*, QVector4D>& modelColorMap)
{
	std::unordered_set<const SceneEntity*> entities;
	for (auto& pair : modelColorMap)
	{
		entities.insert(pair.first);
	}
	//debug directory
#ifdef _DEBUG
	bool prevDebug = Hix::Slicer::Debug::SlicerDebug::getInstance().enableDebug;
	Hix::Slicer::Debug::SlicerDebug::getInstance().enableDebug = false;
#endif

	//slice
	auto layers = SlicingEngine::sliceEntities(entities, 0.1);
	_layerMeshes.reserve(layers.size());

	//for each polytree from slice, triangluate, create mesh
	for (auto& l : layers)
	{
		_layerMeshes.emplace_back();
		auto& mesh = _layerMeshes.back();
		for (auto& s : l.slices)
		{
			PolytreeCDT polycdt(s.polytree.get());
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


#ifdef _DEBUG
	Hix::Slicer::Debug::SlicerDebug::getInstance().enableDebug = prevDebug;
#endif
}

Hix::Features::CrossSectionPlane::~CrossSectionPlane()
{
	//sicne mesh will be deleted here
	setMesh(nullptr);
}

void Hix::Features::CrossSectionPlane::showLayer(double zHeight)
{
	size_t layer = zHeight / Hix::Application::ApplicationManager::getInstance().settings().sliceSetting.layerHeight;
	layer = std::min(_layerMeshes.size() - 1, layer);
	setMesh(&_layerMeshes[layer]);
	//set z
	//set to identity
	qDebug() << "layer:" << layer;
	transform().setMatrix(QMatrix4x4());
	transform().setTranslation(QVector3D(0, 0, zHeight));
}


QVector4D Hix::Features::CrossSectionPlane::getPrimitiveColorCode(const Hix::Engine3D::Mesh* mesh, FaceConstItr faceItr)
{
	throw std::runtime_error("Hix::Support::CylindricalRaft::getPrimitiveColorCode not implemented");
	return QVector4D();
}