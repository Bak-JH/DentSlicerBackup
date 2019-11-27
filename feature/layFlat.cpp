#include "layFlat.h"
#include "qmlmanager.h"
using namespace Hix::Features;


Hix::Features::LayFlatMode::LayFlatMode(const std::unordered_set<GLModel*>& selectedModels)
	: PPShaderFeature(selectedModels)
{
}

Hix::Features::LayFlatMode::~LayFlatMode()
{
}

void Hix::Features::LayFlatMode::faceSelected(GLModel* selected, const Hix::Engine3D::FaceConstItr& selectedFace, const Hix::Input::MouseEventData& mouse, const Qt3DRender::QRayCasterHit& hit)
{
	auto listed = selected->getRootModel();
	auto neighbors = selected->getMesh()->findNearSimilarFaces(selectedFace.localFn(), selectedFace);
	PPShaderFeature::colorFaces(selected, neighbors);
	auto worldFn = selectedFace.worldFn();
	_args[selected] = listed->vectorToLocal(worldFn);
	isReady = true;
}

std::unique_ptr<Hix::Features::FeatureContainer> Hix::Features::LayFlatMode::applyLayFlat()
{
	if (_args.empty())
		return nullptr;

	std::unique_ptr<Hix::Features::FeatureContainer> container = std::make_unique<FeatureContainer>();
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

	_prevRotation = new QQuaternion(selectedModel->transform().rotation());
	_prevAabb = new Bounds3D(selectedModel->aabb());

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
	_model->transform().setRotation(*_prevRotation);
	_model->aabb() = *_prevAabb;
	_model->updateMesh();
	_model->setZToBed();
}

Hix::Features::LayFlat::~LayFlat()
{
}
