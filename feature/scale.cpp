#include "scale.h"

Hix::Features::ScaleMode::ScaleMode(const std::unordered_set<GLModel*>& selectedModels)
	: _targetModels(selectedModels)
{
}

Hix::Features::ScaleMode::~ScaleMode()
{
}

Hix::Features::FeatureContainerFlushSupport* Hix::Features::ScaleMode::applyScale(QVector3D scale)
{
	Hix::Features::FeatureContainerFlushSupport* container = new FeatureContainerFlushSupport();
	for (auto& target : _targetModels)
		container->addFeature(new Scale(target, scale));

	return container;
}





Hix::Features::Scale::Scale(GLModel* targetModel, QVector3D& scale)
	: _model(targetModel)
{
	_prevMatrix = targetModel->transform().matrix();
	_prevAabb = targetModel->aabb();
	targetModel->scaleModel(scale);
	targetModel->scaleDone();
	qmlManager->sendUpdateModelInfo();
}

Hix::Features::Scale::~Scale()
{
	_model = nullptr;
	delete _model;
}

void Hix::Features::Scale::undoImpl()
{
	_nextMatrix = _model->transform().matrix();
	_nextAabb = _model->aabb();

	_model->transform().setMatrix(_prevMatrix);
	_model->aabb() = _prevAabb;
	qmlManager->sendUpdateModelInfo();
	_model->updateMesh();
}

void Hix::Features::Scale::redoImpl()
{
	_model->transform().setMatrix(_nextMatrix);
	_model->aabb() = _nextAabb;
	qmlManager->sendUpdateModelInfo();
	_model->updateMesh();
}
