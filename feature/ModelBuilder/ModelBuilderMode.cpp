#include "ModelBuilderMode.h"
#include <QFileDialog>

#include "../../glmodel.h"
#include "../addModel.h"
#include <unordered_set>
#include "TwoManifoldBuilder.h"
#include "../repair/meshrepair.h"
#include "application/ApplicationManager.h"

constexpr float ZMARGIN = 10;


Hix::Features::ModelBuilderMode::ModelBuilderMode(): _topPlane(Hix::Application::ApplicationManager::getInstance().sceneManager().total(), true), _bottPlane(Hix::Application::ApplicationManager::getInstance().sceneManager().total(), true)
{
	auto fileUrl = QFileDialog::getOpenFileUrl(nullptr, "Select scanned surface file", QUrl(), "3D Model file (*.stl)");
	auto fileName = fileUrl.fileName();
	if (fileName.isEmpty())
	{
		return;
	}


	//Hix::Application::ApplicationManager::getInstance().openProgressPopUp();
	auto mesh = new Mesh();
	if (fileName != "" && (fileName.contains(".stl") || fileName.contains(".STL"))) {
		FileLoader::loadMeshSTL(mesh, fileUrl);
	}
	else if (fileName != "" && (fileName.contains(".obj") || fileName.contains(".OBJ"))) {
		FileLoader::loadMeshOBJ(mesh, fileUrl);
	}
	fileName = GLModel::filenameToModelName(fileName.toStdString());
	//Hix::Application::ApplicationManager::getInstance().setProgress(0.1);
	_model.reset(new GLModel(Hix::Application::ApplicationManager::getInstance().partManager().modelRoot(), mesh, fileName, nullptr));
	_model->setZToBed();
	_model->moveModel(QVector3D(0, 0, ZMARGIN));
	float cutPlane, botPlane;
	QQuaternion rotation;
	guessOrientation(*_model->getMeshModd(), cutPlane, botPlane, rotation);
	_model->transform().setRotation(rotation);
	_model->updateRecursiveAabb();
	_zLength = _model->aabb().lengthZ() + ZMARGIN;
	auto translationZ = _model->transform().translation().z();
	cutPlane += translationZ;
	botPlane += translationZ;
	_topPlane.transform().setTranslation(QVector3D(0, 0, cutPlane - ZMARGIN));
	_bottPlane.transform().setTranslation(QVector3D(0, 0, botPlane - ZMARGIN));
	//QMetaObject::invokeMethod(Hix::Application::ApplicationManager::getInstance().modelBuilderPopup, "setRangeSliderValueFirst", Q_ARG(QVariant, ((botPlane - ZMARGIN) / _zLength) * 1.8));
	//QMetaObject::invokeMethod(Hix::Application::ApplicationManager::getInstance().modelBuilderPopup, "setRangeSliderValueSecond", Q_ARG(QVariant, ((cutPlane - ZMARGIN)/ _zLength) * 1.8));


	std::unordered_set<GLModel*> models { _model.get() };
	_rotateMode.reset(new RotateModeNoUndo(models));
	//Hix::Application::ApplicationManager::getInstance().setProgress(1.0);
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
	TwoManifoldBuilder* builder = new TwoManifoldBuilder(*_model->getMeshModd(), _model->modelName(), cuttingPlane, bottomPlane);
	Hix::Application::ApplicationManager::getInstance().taskManager().enqueTask(builder);
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
