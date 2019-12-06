#include "move.h"
#include "ui/MoveWidget.h"
#include "qmlmanager.h"

Hix::Features::MoveMode::MoveMode(const std::unordered_set<GLModel*>& targetModels, Input::RayCastController* controller)
	: WidgetMode(targetModels, controller)
{
	_widget.addWidget(std::make_unique<Hix::UI::MoveWidget>(QVector3D(1, 0, 0), &_widget));
	_widget.addWidget(std::make_unique<Hix::UI::MoveWidget>(QVector3D(0, 1, 0), &_widget));
	_widget.setVisible(true);
}

Hix::Features::MoveMode::~MoveMode()
{
}

void Hix::Features::MoveMode::featureStarted()
{
	_moveContainer = new FeatureContainer();
	for (auto& target : _targetModels)
		_moveContainer->addFeature(new Move(target));
}

void Hix::Features::MoveMode::featureEnded()
{
	for (auto& each : _targetModels)
	{
		qmlManager->featureHistoryManager().addFeature(_moveContainer);
		each->moveDone();
		qmlManager->sendUpdateModelInfo();
	}
	_widget.updatePosition();
}

Hix::Features::Move::Move(GLModel* target) : _target(target)
{
	_prevMatrix = target->transform().matrix();
	_prevAabb = target->aabb();
	qDebug() << _prevMatrix;
}

Hix::Features::Move::~Move()
{
}

void Hix::Features::Move::undo()
{
	_target->transform().setMatrix(_prevMatrix);
	_target->aabb() = _prevAabb;
	qmlManager->cameraViewChanged();
}
