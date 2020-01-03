#include "ModelBuilderMode.h"
#include <QFileDialog>
#include "../../qmlmanager.h"
#include "../../glmodel.h"
#include "../addModel.h"
#include <unordered_set>
#include "TwoManifoldBuilder.h"
#include "../repair/meshrepair.h"
constexpr float ZMARGIN = 10;


Hix::Features::ModelBuilderMode::ModelBuilderMode(): _topPlane(qmlManager->total, true), _bottPlane(qmlManager->total, true)
{
	auto fileUrl = QFileDialog::getOpenFileUrl(nullptr, "Select scanned surface file", QUrl(), "3D Model file (*.stl)");
	auto fileName = fileUrl.fileName();
	if (fileName.isEmpty())
	{
		return;
	}


	qmlManager->openProgressPopUp();
	auto mesh = new Mesh();
	if (fileName != "" && (fileName.contains(".stl") || fileName.contains(".STL"))) {
		FileLoader::loadMeshSTL(mesh, fileUrl);
	}
	else if (fileName != "" && (fileName.contains(".obj") || fileName.contains(".OBJ"))) {
		FileLoader::loadMeshOBJ(mesh, fileUrl);
	}
	fileName = qmlManager->filenameToModelName(fileName.toStdString());
	qmlManager->setProgress(0.1);
	_model.reset(new GLModel(qmlManager->models, mesh, fileName, nullptr));
	_model->setHitTestable(true);
	_model->setZToBed();
	_model->moveModel(QVector3D(0, 0, ZMARGIN));
	_builder = new TwoManifoldBuilder(*_model->getMeshModd());
	float cutPlane, botPlane;
	QQuaternion rotation;
	_builder->autogenOrientation(cutPlane, botPlane, rotation);
	_model->transform().setRotation(rotation);
	_model->updateRecursiveAabb();
	_zLength = _model->aabb().lengthZ() + ZMARGIN;
	auto translationZ = _model->transform().translation().z();
	cutPlane += translationZ;
	botPlane += translationZ;
	_topPlane.transform().setTranslation(QVector3D(0, 0, cutPlane));
	_bottPlane.transform().setTranslation(QVector3D(0, 0, botPlane));
	QMetaObject::invokeMethod(qmlManager->modelBuilderPopup, "setRangeSliderValueFirst", Q_ARG(QVariant, (botPlane/ _zLength) * 1.8));
	QMetaObject::invokeMethod(qmlManager->modelBuilderPopup, "setRangeSliderValueSecond", Q_ARG(QVariant, (cutPlane/ _zLength) * 1.8));



	std::unordered_set<GLModel*> models { _model.get() };
	_rotateMode.reset(new RotateModeNoUndo(models, &qmlManager->getRayCaster()));
	qmlManager->setProgress(1.0);
}

Hix::Features::ModelBuilderMode::~ModelBuilderMode()
{
}

void Hix::Features::ModelBuilderMode::build()
{
	auto cuttingPlane = _topPlane.transform().translation().z();
	auto bottomPlane = _bottPlane.transform().translation().z();
	auto aabb0 = _model->aabb();
	_model->flushTransform();
	auto aabb1 = _model->aabb();
	_builder->buildModel(_model->modelName(), cuttingPlane, bottomPlane);
	qmlManager->featureHistoryManager().addFeature(_builder);
	if (Hix::Features::isRepairNeeded(_builder->result()->getMesh()))
	{
		qmlManager->setProgressText("Repairing mesh.");
		std::unordered_set<GLModel*> repairModels;
		repairModels.insert(_builder->result());
		MeshRepair rapairModels(repairModels);
	}
	_model->setMesh(nullptr);
}

void Hix::Features::ModelBuilderMode::getSliderSignalTop(double value)
{
	_topPlane.transform().setTranslation(QVector3D(0, 0, _zLength * value / 1.8));
}

void Hix::Features::ModelBuilderMode::getSliderSignalBot(double value)
{
	_bottPlane.transform().setTranslation(QVector3D(0, 0, _zLength * value / 1.8));
}
