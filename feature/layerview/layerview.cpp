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
	RangeSliderMode(0, Hix::Application::ApplicationManager::getInstance().partManager().selectedBound().zMax()),
	_models(Hix::Application::ApplicationManager::getInstance().partManager().selectedModels()),
	_topCrossSectionPlane(Hix::Application::ApplicationManager::getInstance().sceneManager().total()),
	_botCrossSectionPlane(Hix::Application::ApplicationManager::getInstance().sceneManager().total())
{
	if (Hix::Application::ApplicationManager::getInstance().partManager().selectedModels().empty())
	{
		Hix::Application::ApplicationManager::getInstance().modalDialogManager().needToSelectModels();
		return;
	}

	slider().setUpperValue(slider().getMax());
	slider().setVisible(true);

	//change shader, change support, raft to same color as model
	for (auto& model : _models)
	{
		model->setMaterialMode(Hix::Render::ShaderMode::LayerMode);
		model->updateMesh();
		model->setMaterialParamter("fuckingStuipidWorldMatrix", QVariant::fromValue(model->toRootMatrix()));
		model->setMaterialParamter("minHeight", QVariant::fromValue((float)slider().lowerValue()));
		model->setMaterialParamter("maxHeight", QVariant::fromValue((float)slider().upperValue()));
		_modelColorMap[model] = Hix::Render::Colors::Selected;
	}
	auto supports = Hix::Application::ApplicationManager::getInstance().supportRaftManager().modelAttachedSupports(_models);
	for (auto& s : supports)
	{
		s->setMaterialColor(Hix::Render::Colors::Selected);
		s->setMaterialMode(Hix::Render::ShaderMode::LayerMode);
		s->updateMesh();
		s->setMaterialParamter("fuckingStuipidWorldMatrix", QVariant::fromValue(s->toRootMatrix()));
		s->setMaterialParamter("minHeight", QVariant::fromValue((float)slider().lowerValue()));
		s->setMaterialParamter("maxHeight", QVariant::fromValue((float)slider().upperValue()));
		_modelColorMap[s] = Hix::Render::Colors::Support;

	}
	auto raft = Hix::Application::ApplicationManager::getInstance().supportRaftManager().raftModel();
	if(raft)
	{
		raft->setMaterialColor(Hix::Render::Colors::Selected);
		raft->setMaterialMode(Hix::Render::ShaderMode::LayerMode);
		raft->updateMesh();
		raft->setMaterialParamter("fuckingStuipidWorldMatrix", QVariant::fromValue(raft->toRootMatrix()));
		raft->setMaterialParamter("minHeight", QVariant::fromValue((float)slider().lowerValue()));
		raft->setMaterialParamter("maxHeight", QVariant::fromValue((float)slider().upperValue()));
		_modelColorMap[raft] = Hix::Render::Colors::Raft;
	}

	QObject::connect(&slider(), &Hix::QML::RangeSlideBarShell::upperValueChanged, [this]() {
		_topCrossSectionPlane.showLayer(slider().upperValue(), true);
		float h = slider().upperValue();
		for (auto& pair : _modelColorMap)
		{
			pair.first->setMaterialParamter("maxHeight", QVariant::fromValue(h));
		}
		});
	
	QObject::connect(&slider(), &Hix::QML::RangeSlideBarShell::lowerValueChanged, [this]() {
		_botCrossSectionPlane.showLayer(slider().lowerValue(), false);
		float h = slider().lowerValue();
		for (auto& pair : _modelColorMap)
		{
			pair.first->setMaterialParamter("minHeight", QVariant::fromValue(h));
		}
		});

	//prepare layers
	Hix::Application::ApplicationManager::getInstance().taskManager().enqueTask(std::make_unique<LayerviewPrep>(_modelColorMap, _topCrossSectionPlane, true));
	Hix::Application::ApplicationManager::getInstance().taskManager().enqueTask(std::make_unique<LayerviewPrep>(_modelColorMap, _botCrossSectionPlane, false));
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
	Hix::Features::CrossSectionPlane& crossSec, bool isTop): _crossSec(crossSec), _modelColorMap(selected), _isTop(isTop)
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
		Bounds3D bound = Hix::Engine3D::combineBounds(entities);
		float h = _isTop ? bound.zMax() : bound.zMin();
		_crossSec.showLayer(h, _isTop);
	});
}
