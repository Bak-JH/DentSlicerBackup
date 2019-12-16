#include "layFlat.h"
#include "qmlmanager.h"
using namespace Hix::Features;


Hix::Features::LayFlatMode::LayFlatMode(const std::unordered_set<GLModel*>& selectedModels)
	: PPShaderMode(selectedModels)
{
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
	_args[selected] = listed->vectorToLocal(worldFn);
	isReady = true;
}

Hix::Features::FeatureContainer* Hix::Features::LayFlatMode::applyLayFlat()
{
	if (_args.empty())
		return nullptr;

	Hix::Features::FeatureContainer* container = new FeatureContainer();
	for (auto& each : _args)
	{
		container->addFeature(new LayFlat(each.first, each.second, isReady));
	}

	_args.clear();

	return container;
}





Hix::Features::LayFlat::LayFlat(GLModel* selectedModel, QVector3D normal, bool isReady) : _model(selectedModel)
{
	if (!isReady)
		return;

	_prevMatrix = selectedModel->transform().matrix();
	_prevAabb = selectedModel->aabb();

	constexpr QVector3D worldBot(0, 0, -1);
	QVector3D localBotNorml = selectedModel->vectorToLocal(worldBot);
	auto rot = QQuaternion::rotationTo(normal, localBotNorml);

	selectedModel->unselectMeshFaces();
	selectedModel->updateMesh(true);

	selectedModel->transform().setRotation(selectedModel->transform().rotation() * rot);
	selectedModel->updateRecursiveAabb();
	selectedModel->setZToBed();
	qmlManager->resetLayflat();
}

void Hix::Features::LayFlat::undo()
{
	auto currMatrix = _model->transform().matrix();
	auto currAabb = _model->aabb();

	_model->transform().setMatrix(_prevMatrix);
	_model->aabb() = _prevAabb;
	qmlManager->cameraViewChanged();
	_model->setZToBed();
	_model->unselectMeshFaces();
	_model->updateMesh(true);

	_prevMatrix = currMatrix;
	_prevAabb = currAabb;
}

void Hix::Features::LayFlat::redo()
{
	auto currMatrix = _model->transform().matrix();
	auto currAabb = _model->aabb();

	_model->transform().setMatrix(_prevMatrix);
	_model->aabb() = _prevAabb;
	qmlManager->cameraViewChanged();
	_model->setZToBed();
	_model->unselectMeshFaces();
	_model->updateMesh(true);

	_prevMatrix = currMatrix;
	_prevAabb = currAabb;
}

Hix::Features::LayFlat::~LayFlat()
{
}
