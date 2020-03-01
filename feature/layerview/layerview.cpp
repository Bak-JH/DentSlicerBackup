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



Hix::Features::LayerView::LayerView(const std::unordered_set<GLModel*>& selectedModels, Hix::Engine3D::Bounds3D bound):
	_models(selectedModels), _crossSectionPlane(Hix::Application::ApplicationManager::getInstance().sceneManager().total()), _modelsBound(bound)
{
	SlicingEngine::Result result;
	SliceExport se(_models);
	try
	{
		se.run();
		result = se.getResult();
	}
	catch (...)
	{
		qDebug() << "slice export failed";
		return;
	}

	//prepare layers
	for (auto& model : _models)
	{
		//model->changeViewMode(VIEW_MODE_LAYER);
		model->updateModelMesh();
		model->meshMaterial().setParameterValue("fuckingStuipidWorldMatrix", QVariant::fromValue(model->toRootMatrix()));
	}
	//Hix::Application::ApplicationManager::getInstance().layerViewPopup->setProperty("visible", true);
	//Hix::Application::ApplicationManager::getInstance().layerViewSlider->setProperty("visible", true);
	//QMetaObject::invokeMethod(Hix::Application::ApplicationManager::getInstance().layerViewSlider, "setThickness", Q_ARG(QVariant, (scfg->layer_height)));
	//QMetaObject::invokeMethod(Hix::Application::ApplicationManager::getInstance().layerViewSlider, "setHeight",
	//	Q_ARG(QVariant,	(_modelsBound.zMax() - _modelsBound.zMin() + scfg->raft_thickness + scfg->support_base_height)));

	_crossSectionPlane.enablePlane(true);
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

void Hix::Features::LayerView::crossSectionSliderSignal(int value)
{
	float zlength = _modelsBound.lengthZ();
	_crossSectionPlane.transform().setTranslation(QVector3D(0, 0, _modelsBound.zMin() + (double)value * zlength / (double)_maxLayer));

	//_transform.setTranslation(QVector3D(0, 0, value * scfg->layer_height - scfg->raft_thickness - scfg->support_base_height));


	QDir dir(QDir::tempPath() + "_export");
	if (dir.exists()) {
		QString filename = dir.path() + "/" + QString::number(value) + ".svg";
		qDebug() << filename;
		_crossSectionPlane.loadTexture(filename);
	}


	float h = Hix::Application::ApplicationManager::getInstance().settings().sliceSetting.layerHeight* value + _modelsBound.zMin();
	// change phong material of original model
	for (auto& model : _models)
	{
		model->meshMaterial().setParameterValue("height", QVariant::fromValue(h));
	}

}
