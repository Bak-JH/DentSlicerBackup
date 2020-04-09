#include "ModelBuilderMode.h"
#include <QFileDialog>

#include "../../glmodel.h"
#include "../addModel.h"
#include <unordered_set>
#include "TwoManifoldBuilder.h"
#include "../repair/meshrepair.h"
#include "application/ApplicationManager.h"
#include "../widget/RotateWidget.h"
#include <fstream>
#include <boost/algorithm/string.hpp>

constexpr float ZMARGIN = 3;

const QUrl MB_POPUP = QUrl("qrc:/Qml/FeaturePopup/PopupModelBuild.qml");




void Hix::Features::ModelBuilderMode::featureStarted()
{
}

void Hix::Features::ModelBuilderMode::featureEnded()
{

	_model->updateRecursiveAabb();
	updatePosition();
}

QVector3D Hix::Features::ModelBuilderMode::getWidgetPosition()
{
	return 	_model->recursiveAabb().centre();
}



Hix::Features::ModelBuilderMode::ModelBuilderMode(): 
	DialogedMode(MB_POPUP), RangeSliderMode(0, 1),
	_topPlane(Hix::Application::ApplicationManager::getInstance().sceneManager().total(), true),
	_bottPlane(Hix::Application::ApplicationManager::getInstance().sceneManager().total(), true)
{


	auto fileUrl = QFileDialog::getOpenFileUrl(nullptr, "Select scanned surface file", QUrl(), "3D Model file (*.stl)");
	auto fileName = fileUrl.fileName();
	if (fileName.isEmpty())
	{
		return;
	}
	Hix::Application::ApplicationManager::getInstance().taskManager().enqueTask(std::make_unique<MBPrep>(this, fileUrl));


	QObject::connect(&slider(), &Hix::QML::RangeSlideBarShell::lowerValueChanged, [this]() {
		_bottPlane.transform().setMatrix(QMatrix4x4());
		_bottPlane.transform().setTranslation(QVector3D(0, 0, slider().lowerValue()));
	});

	QObject::connect(&slider(), &Hix::QML::RangeSlideBarShell::upperValueChanged, [this]() {
		_topPlane.transform().setMatrix(QMatrix4x4());
		_topPlane.transform().setTranslation(QVector3D(0, 0, slider().upperValue()));
		});

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

void Hix::Features::ModelBuilderMode::applyButtonClicked()
{
	build();
}

Hix::Features::MBPrep::MBPrep(ModelBuilderMode* mode, QUrl fileUrl): _mode(mode), _fileUrl(fileUrl)
{
}

Hix::Features::MBPrep::~MBPrep()
{
}

void Hix::Features::MBPrep::run()
{
	auto fileName = _fileUrl.fileName();
	std::filesystem::path filePath(_fileUrl.toLocalFile().toStdWString());
	std::fstream file(filePath);
	auto mesh = new Mesh();
	if (boost::iequals(filePath.extension().string(), ".stl")) {

		if (!FileLoader::loadMeshSTL(mesh, file))
		{
			std::fstream fileBinary(filePath, std::ios_base::in | std::ios_base::binary);
			FileLoader::loadMeshSTL_binary(mesh, fileBinary);
		}
	}
	else if (boost::iequals(filePath.extension().string(), ".obj")) {
		FileLoader::loadMeshOBJ(mesh, file);
	}
	fileName = GLModel::filenameToModelName(fileName.toStdString());
	mesh->centerMesh();
	postUIthread([this, &mesh, &fileName]() {
		_mode->_model.reset(new GLModel(Hix::Application::ApplicationManager::getInstance().partManager().modelRoot(), mesh, fileName, nullptr));

	});
	float cutPlane, botPlane;
	QQuaternion rotation;
	guessOrientation(*_mode->_model->getMeshModd(), cutPlane, botPlane, rotation);

	postUIthread([this, &cutPlane, &botPlane, &rotation]() {
		_mode->_model->transform().setRotation(rotation);
		_mode->_model->updateRecursiveAabb();
		auto zTranslate = _mode->_model->setZToBed();

		_mode->_model->moveModel(QVector3D(0, 0, ZMARGIN));
		zTranslate += ZMARGIN;
		_mode->_model->updateRecursiveAabb();

		auto rAABB = _mode->_model->recursiveAabb();
		auto modelMaxRadius = rAABB.bbMaxRadius();
		auto modelCentre = rAABB.centre();
		_mode->slider().setMin(modelCentre.z() -modelMaxRadius);
		_mode->slider().setMax(modelCentre.z() + modelMaxRadius);
		auto upSliderVal = cutPlane + zTranslate;
		auto lowSliderVal = botPlane + zTranslate;
		
		_mode->slider().setUpperValue(upSliderVal);
		_mode->slider().setLowerValue(lowSliderVal);
		_mode->_topPlane.transform().setTranslation(QVector3D(0, 0, upSliderVal));
		_mode->_bottPlane.transform().setTranslation(QVector3D(0, 0, lowSliderVal));
		std::unordered_set<GLModel*> models{ _mode->_model.get() };

		//widget stuff
		_mode->_widget.addWidget(std::make_unique<Hix::UI::RotateWidget>(QVector3D(1, 0, 0), &_mode->_widget, models));
		_mode->_widget.addWidget(std::make_unique<Hix::UI::RotateWidget>(QVector3D(0, 1, 0), &_mode->_widget, models));
		_mode->_widget.addWidget(std::make_unique<Hix::UI::RotateWidget>(QVector3D(0, 0, 1), &_mode->_widget, models));
		_mode->_widget.setVisible(true);


		_mode->updatePosition();
		_mode->slider().setVisible(true);
		});
}
