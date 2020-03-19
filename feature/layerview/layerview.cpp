#include "layerview.h"
#include "../../application/ApplicationManager.h"
#include "../sliceExport.h"
#include "../cut/modelcut.h"
#include "../../glmodel.h"
#include "../../support/SupportModel.h"
using namespace Hix;
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
	slider().setValue(slider().getMax());
	slider().setVisible(true);


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
		s->setMaterialMode(Hix::Render::ShaderMode::LayerMode);
		s->updateMesh();
		s->setMaterialParamter("fuckingStuipidWorldMatrix", QVariant::fromValue(s->toRootMatrix()));
		s->setMaterialParamter("height", QVariant::fromValue((float)slider().getMax()));
		_modelColorMap[s] = Hix::Render::Colors::Support;

	}
	QObject::connect(&slider(), &Hix::QML::SlideBarShell::valueChanged, [this]() {
		_crossSectionPlane.showLayer(slider().getValue());
		float h = slider().getValue();
		for (auto& model : _models)
		{
			model->setMaterialParamter("height", QVariant::fromValue(h));
		}
		});
	
	//prepare layers
	Hix::Application::ApplicationManager::getInstance().taskManager().enqueTask(std::make_unique<LayerviewPrep>(_models, _crossSectionPlane));


}

Hix::Features::LayerView::~LayerView()
{
	for (auto& model : _models)
	{
		model->setMaterialMode(Hix::Render::ShaderMode::SingleColor);
		model->updateMesh();
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
	postUIthread([this]() {
		float h = Hix::Engine3D::combineBounds(_models).zMax();
		_crossSec.showLayer(h);
		});
}
