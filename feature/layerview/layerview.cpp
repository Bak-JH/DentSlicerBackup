#include "layerview.h"
#include "../../qmlmanager.h"
#include "../sliceExport.h"
using namespace Hix;
using namespace Hix::Features;
using namespace Hix::Features::Cut;
using namespace Hix::Engine3D;
using namespace Hix::Slicer;
using namespace ClipperLib;



Hix::Features::LayerView::LayerView(const std::unordered_set<GLModel*>& selectedModels, Hix::Engine3D::Bounds3D bound):
	_models(selectedModels), _crossSectionPlane(qmlManager->total), _modelsBound(bound)
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
		model->changeViewMode(VIEW_MODE_LAYER);
		model->updateModelMesh();
		model->meshMaterial().setParameterValue("fuckingStuipidWorldMatrix", QVariant::fromValue(model->toRootMatrix()));
	}
	qmlManager->layerViewPopup->setProperty("visible", true);
	qmlManager->layerViewSlider->setProperty("visible", true);
	QMetaObject::invokeMethod(qmlManager->layerViewSlider, "setThickness", Q_ARG(QVariant, (scfg->layer_height)));
	QMetaObject::invokeMethod(qmlManager->layerViewSlider, "setHeight",
		Q_ARG(QVariant,	(_modelsBound.zMax() - _modelsBound.zMin() + scfg->raft_thickness + scfg->support_base_height)));

	_crossSectionPlane.enablePlane(true);
	QVariant maxLayerCount;
	QMetaObject::invokeMethod(qmlManager->layerViewSlider, "getMaxLayer", Qt::DirectConnection, Q_RETURN_ARG(QVariant, maxLayerCount));
	_maxLayer = maxLayerCount.toInt();
	crossSectionSliderSignal(_maxLayer);

}

Hix::Features::LayerView::~LayerView()
{
	for (auto& model : _models)
	{
		model->changeViewMode(VIEW_MODE_OBJECT);
		model->updateModelMesh();
	}
	qmlManager->layerViewPopup->setProperty("visible", false);
	qmlManager->layerViewSlider->setProperty("visible", false);
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


	float h = scfg->layer_height * value + _modelsBound.zMin();
	// change phong material of original model
	for (auto& model : _models)
	{
		model->meshMaterial().setParameterValue("height", QVariant::fromValue(h));
	}

}
