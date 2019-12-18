#include "rotate.h"
#include "qmlmanager.h"
#include "ui/RotateWidget.h"

Hix::Features::RotateMode::RotateMode(const std::unordered_set<GLModel*>& targetModels, Hix::Input::RayCastController* controller) 
		: WidgetMode(targetModels, controller)
{
	_widget.addWidget(std::make_unique<Hix::UI::RotateWidget>(QVector3D(1, 0, 0), &_widget));
	_widget.addWidget(std::make_unique<Hix::UI::RotateWidget>(QVector3D(0, 1, 0), &_widget));
	_widget.addWidget(std::make_unique<Hix::UI::RotateWidget>(QVector3D(0, 0, 1), &_widget));
	_widget.setVisible(true);
}

Hix::Features::RotateMode::~RotateMode()
{

}

void Hix::Features::RotateMode::featureStarted()
{
	_rotateContainer = new FeatureContainerFlushSupport();
	for (auto& target : _targetModels)
		_rotateContainer->addFeature(new Rotate(target));

}

void Hix::Features::RotateMode::featureEnded()
{
	if (!_rotateContainer->empty())
		qmlManager->featureHistoryManager().addFeature(_rotateContainer);
	for (auto& each : _targetModels)
	{
		each->rotateDone();
		qmlManager->sendUpdateModelInfo();
	}
	_widget.updatePosition();
}

Hix::Features::FeatureContainerFlushSupport* Hix::Features::RotateMode::applyRotate(const QQuaternion& rot)
{
	Hix::Features::FeatureContainerFlushSupport* container = new FeatureContainerFlushSupport();
	for (auto& target : _targetModels)
		container->addFeature(new Rotate(target, rot));
	return container;
}

Hix::Features::Rotate::Rotate(GLModel* target) : _model(target)
{
	_prevMatrix = target->transform().matrix();
	_prevAabb = target->aabb();
}

Hix::Features::Rotate::Rotate(GLModel* target, const QQuaternion& rot) : _model(target)
{
	_prevMatrix = target->transform().matrix();
	_prevAabb = target->aabb();
	target->rotateModel(rot);

	if (qmlManager->isActive<Hix::Features::WidgetMode>())
		qmlManager->cameraViewChanged();
}

Hix::Features::Rotate::~Rotate()
{

}

void Hix::Features::Rotate::undo()
{
	_nextMatrix = _model->transform().matrix();
	_nextAabb = _model->aabb();

	_model->transform().setMatrix(_prevMatrix);
	_model->aabb() = _prevAabb;
	qmlManager->cameraViewChanged();
	_model->updateMesh();
}

void Hix::Features::Rotate::redo()
{
	_model->transform().setMatrix(_nextMatrix);
	_model->aabb() = _nextAabb;
	qmlManager->cameraViewChanged();
	_model->updateMesh();
}
