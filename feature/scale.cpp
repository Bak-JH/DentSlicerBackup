#include "scale.h"
#include "qmlmanager.h"

Hix::Features::ScaleMode::ScaleMode(): _targetModels(qmlManager->getSelectedModels())
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
	: _model(targetModel), _scale(scale)
{}

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

void Hix::Features::Scale::runImpl()
{
	_prevMatrix = _model->transform().matrix();
	_prevAabb = _model->aabb();
	_model->scaleModel(_scale);
	_model->scaleDone();
	qmlManager->sendUpdateModelInfo();
}
