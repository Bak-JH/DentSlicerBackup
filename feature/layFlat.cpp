#include "layFlat.h"
#include "../glmodel.h"
#include "application/ApplicationManager.h"
using namespace Hix::Features;

const QUrl LAYFLAT_POPUP_URL = QUrl("qrc:/Qml/FeaturePopup/PopupLayFlat.qml");
Hix::Features::LayFlatMode::LayFlatMode()
	: PPShaderMode(Hix::Application::ApplicationManager::getInstance().partManager().selectedModels()), DialogedMode(LAYFLAT_POPUP_URL)
{
	if (Hix::Application::ApplicationManager::getInstance().partManager().selectedModels().empty())
	{
		Hix::Application::ApplicationManager::getInstance().modalDialogManager().needToSelectModels();
		return;
	}
}

Hix::Features::LayFlatMode::~LayFlatMode()
{
}

void Hix::Features::LayFlatMode::faceSelected(GLModel* selected, const Hix::Engine3D::FaceConstItr& selectedFace, const Hix::Input::MouseEventData& mouse, const Qt3DRender::QRayCasterHit& hit)
{
	auto listed = selected->getRootModel();
	auto neighbors = selected->getMesh()->findNearSimilarFaces(selectedFace.localFn(), selectedFace);
	PPShaderMode::colorFaces(selected, neighbors);
	auto worldFn = selectedFace.worldFn();
	_args[listed] = listed->vectorToLocal(worldFn);
}

void Hix::Features::LayFlatMode::applyButtonClicked()
{
	if (_args.empty())
		return;

	Hix::Features::FeatureContainerFlushSupport* container = new FeatureContainerFlushSupport(Hix::Application::ApplicationManager::getInstance().partManager().selectedModels());
	for (auto& each : _args)
	{
		container->addFeature(new LayFlat(each.first, each.second));
	}
	_args.clear();
	Hix::Application::ApplicationManager::getInstance().taskManager().enqueTask(container);
}





Hix::Features::LayFlat::LayFlat(GLModel* selectedModel, QVector3D normal) : _model(selectedModel), _normal(normal)
{
	_progress.setDisplayText("Lay Flat Model");
}

void Hix::Features::LayFlat::undoImpl()
{
	auto currMatrix = _model->transform().matrix();
	auto currAabb = _model->aabb();

	_model->transform().setMatrix(_prevMatrix);
	_model->aabb() = _prevAabb;
	_model->setZToBed();
	_model->unselectMeshFaces();
	_model->updateMesh(true);

	_prevMatrix = currMatrix;
	_prevAabb = currAabb;
}

void Hix::Features::LayFlat::redoImpl()
{
	auto currMatrix = _model->transform().matrix();
	auto currAabb = _model->aabb();

	_model->transform().setMatrix(_prevMatrix);
	_model->aabb() = _prevAabb;
	_model->setZToBed();
	_model->unselectMeshFaces();
	_model->updateMesh(true);

	_prevMatrix = currMatrix;
	_prevAabb = currAabb;
}

void Hix::Features::LayFlat::runImpl()
{
	_prevMatrix = _model->transform().matrix();
	_prevAabb = _model->aabb();

	constexpr QVector3D worldBot(0, 0, -1);
	QVector3D localBotNorml = _model->vectorToLocal(worldBot);
	auto rot = QQuaternion::rotationTo(_normal, localBotNorml);

	_model->unselectMeshFaces();
	_model->updateMesh(true);

	_model->transform().setRotation(_model->transform().rotation() * rot);
	_model->updateRecursiveAabb();
	_model->setZToBed();
}

Hix::Features::LayFlat::~LayFlat()
{
	_model = nullptr;
	delete _model;
}
