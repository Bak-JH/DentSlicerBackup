#include "layerview.h"
#include "../../application/ApplicationManager.h"
#include "../slice/sliceExport.h"
#include "../cut/modelcut.h"
#include "../../glmodel.h"
#include "../../support/SupportModel.h"
#include "../../support/RaftModel.h"

using namespace Hix;
using namespace Hix::Render;
using namespace Hix::Features;
using namespace Hix::Features::Cut;
using namespace Hix::Engine3D;
using namespace Hix::Slicer;
using namespace ClipperLib;



Hix::Features::LayerView::LayerView():
	SliderMode(0, Hix::Application::ApplicationManager::getInstance().partManager().selectedBound().zMax()),
	_models(Hix::Application::ApplicationManager::getInstance().partManager().selectedModels()),
	_crossSectionPlane(Hix::Application::ApplicationManager::getInstance().sceneManager().total())
{
	if (Hix::Application::ApplicationManager::getInstance().partManager().selectedModels().empty())
	{
		Hix::Application::ApplicationManager::getInstance().modalDialogManager().needToSelectModels();
		return;
	}

	slider().setValue(slider().getMax());
	slider().setVisible(true);

	//change shader, change support, raft to same color as model
	for (auto& model : _models)
	{
		model->setMaterialMode(Hix::Render::ShaderMode::LayerMode);
		model->updateMesh();
		model->setMaterialParamter("fuckingStuipidWorldMatrix", QVariant::fromValue(model->toRootMatrix()));
		model->setMaterialParamter("height", QVariant::fromValue((float)slider().getMax()));
		_modelColorMap[model] = Hix::Render::Colors::Selected;
	}
	auto supports = Hix::Application::ApplicationManager::getInstance().supportRaftManager().modelAttachedSupports(_models);
	for (auto& s : supports)
	{
		s->setMaterialColor(Hix::Render::Colors::Selected);
		s->setMaterialMode(Hix::Render::ShaderMode::LayerMode);
		s->updateMesh();
		s->setMaterialParamter("fuckingStuipidWorldMatrix", QVariant::fromValue(s->toRootMatrix()));
		s->setMaterialParamter("height", QVariant::fromValue((float)slider().getMax()));
		_modelColorMap[s] = Hix::Render::Colors::Support;

	}
	auto raft = Hix::Application::ApplicationManager::getInstance().supportRaftManager().raftModel();
	if(raft)
	{
		raft->setMaterialColor(Hix::Render::Colors::Selected);
		raft->setMaterialMode(Hix::Render::ShaderMode::LayerMode);
		raft->updateMesh();
		raft->setMaterialParamter("fuckingStuipidWorldMatrix", QVariant::fromValue(raft->toRootMatrix()));
		raft->setMaterialParamter("height", QVariant::fromValue((float)slider().getMax()));
		_modelColorMap[raft] = Hix::Render::Colors::Raft;
	}

	QObject::connect(&slider(), &Hix::QML::SlideBarShell::valueChanged, [this]() {
		_crossSectionPlane.showLayer(slider().getValue());
		float h = std::floor(slider().getValue() * 10) / 10;
		for (auto& pair : _modelColorMap)
		{
			pair.first->setMaterialParamter("height", QVariant::fromValue(h));
		}
		});
	
	//prepare layers
	Hix::Application::ApplicationManager::getInstance().taskManager().enqueTask(std::make_unique<LayerviewPrep>(_modelColorMap, _crossSectionPlane));


}

Hix::Features::LayerView::~LayerView()
{
	//rever colors and shader
	for (auto& pair : _modelColorMap)
	{
		pair.first->setMaterialMode(Hix::Render::ShaderMode::SingleColor);
		pair.first->setMaterialColor(pair.second);
		pair.first->updateMesh();
	}
}

void Hix::Features::LayerView::onExit()
{
	Hix::Application::ApplicationManager::getInstance().featureManager().setViewModeSwitch(false);

}



Hix::Features::LayerviewPrep::LayerviewPrep(const std::unordered_map<Hix::Render::SceneEntityWithMaterial*, QVector4D>& selected,
	Hix::Features::CrossSectionPlane& crossSec): _crossSec(crossSec), _modelColorMap(selected)
{
}

Hix::Features::LayerviewPrep::~LayerviewPrep()
{
}

void Hix::Features::LayerviewPrep::run()
{
	_crossSec.init(_modelColorMap);
	std::unordered_set<SceneEntity*> entities;
	for (auto& pair : _modelColorMap)
	{
		entities.insert(pair.first);
	}
	postUIthread([this, &entities]() {
		float h = Hix::Engine3D::combineBounds(entities).zMax();
		_crossSec.showLayer(h);
		});
}
