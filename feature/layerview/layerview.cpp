#include "layerview.h"
#include "../../application/ApplicationManager.h"
#include "../sliceExport.h"
#include "../cut/modelcut.h"
#include "../../glmodel.h"
using namespace Hix;
using namespace Hix::Features;
using namespace Hix::Features::Cut;
using namespace Hix::Engine3D;
using namespace Hix::Slicer;
using namespace ClipperLib;



Hix::Features::LayerView::LayerView():
	SliderMode(0, Hix::Application::ApplicationManager::getInstance().partManager().selectedBound().lengthZ()),
	_models(Hix::Application::ApplicationManager::getInstance().partManager().selectedModels()),
	_crossSectionPlane(Hix::Application::ApplicationManager::getInstance().sceneManager().total())
{
	Hix::Application::ApplicationManager::getInstance().taskManager().enqueTask(std::make_unique<LayerviewPrep>(_models, _crossSectionPlane));
	//prepare layers
	for (auto& model : _models)
	{
		//model->changeViewMode(VIEW_MODE_LAYER);
		model->updateModelMesh();
		model->meshMaterial().setParameterValue("fuckingStuipidWorldMatrix", QVariant::fromValue(model->toRootMatrix()));
	}

	QObject::connect(_slideBar.get(), &Hix::QML::SlideBarShell::valueChanged, [this]() {
		_crossSectionPlane.showLayer(_slideBar->getValue());
		});

	//Hix::Application::ApplicationManager::getInstance().layerViewPopup->setProperty("visible", true);
	//Hix::Application::ApplicationManager::getInstance().layerViewSlider->setProperty("visible", true);
	//QMetaObject::invokeMethod(Hix::Application::ApplicationManager::getInstance().layerViewSlider, "setThickness", Q_ARG(QVariant, (Hix::Application::ApplicationManager::getInstance().settings().sliceSetting.layerHeight)));
	//QMetaObject::invokeMethod(Hix::Application::ApplicationManager::getInstance().layerViewSlider, "setHeight",
	//	Q_ARG(QVariant,	(_modelsBound.zMax() - _modelsBound.zMin() + scfg->raft_thickness + scfg->support_base_height)));

	//QVariant maxLayerCount;
	//QMetaObject::invokeMethod(Hix::Application::ApplicationManager::getInstance().layerViewSlider, "getMaxLayer", Qt::DirectConnection, Q_RETURN_ARG(QVariant, maxLayerCount));
	//_maxLayer = maxLayerCount.toInt();
	//crossSectionSliderSignal(_maxLayer);

}

Hix::Features::LayerView::~LayerView()
{
	for (auto& model : _models)
	{
		//model->changeViewMode(VIEW_MODE_OBJECT);
		model->updateModelMesh();
	}
	//Hix::Application::ApplicationManager::getInstance().layerViewPopup->setProperty("visible", false);
	//Hix::Application::ApplicationManager::getInstance().layerViewSlider->setProperty("visible", false);
}

void Hix::Features::LayerView::onExit()
{

}

//void Hix::Features::LayerView::crossSectionSliderSignal(int value)
//{
//	float zlength = _modelsBound.lengthZ();
//	_crossSectionPlane.transform().setTranslation(QVector3D(0, 0, _modelsBound.zMin() + (double)value * zlength / (double)_maxLayer));
//
//	//_transform.setTranslation(QVector3D(0, 0, value * Hix::Application::ApplicationManager::getInstance().settings().sliceSetting.layerHeight - scfg->raft_thickness - scfg->support_base_height));
//
//
//	QDir dir(QDir::tempPath() + "_export");
//	if (dir.exists()) {
//		QString filename = dir.path() + "/" + QString::number(value) + ".svg";
//		qDebug() << filename;
//		_crossSectionPlane.loadTexture(filename);
//	}
//
//
//	float h = Hix::Application::ApplicationManager::getInstance().settings().sliceSetting.layerHeight* value + _modelsBound.zMin();
//	// change phong material of original model
//	for (auto& model : _models)
//	{
//		model->meshMaterial().setParameterValue("height", QVariant::fromValue(h));
//	}
//
//}

Hix::Features::LayerviewPrep::LayerviewPrep(const std::unordered_set<GLModel*>& selected, Hix::Features::CrossSectionPlane& crossSec): _crossSec(crossSec), _models(selected)
{
}

Hix::Features::LayerviewPrep::~LayerviewPrep()
{
}

void Hix::Features::LayerviewPrep::run()
{
	_crossSec.init(_models);
}
