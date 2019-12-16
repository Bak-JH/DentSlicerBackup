#include "move.h"
#include "ui/MoveWidget.h"
#include "qmlmanager.h"

Hix::Features::MoveMode::MoveMode
(const std::unordered_set<GLModel*>& targetModels, Input::RayCastController* controller)
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
	qmlManager->featureHistoryManager().addFeature(_moveContainer);

	for (auto& each : _targetModels)
		each->moveDone();
	
	qmlManager->sendUpdateModelInfo();
	_widget.updatePosition();
}

Hix::Features::FeatureContainer* Hix::Features::MoveMode::applyMove(const QVector3D& to)
{
	Hix::Features::FeatureContainer* container = new FeatureContainer();
	for (auto& target : _targetModels)
		container->addFeature(new Move(target, to));
	return container;
}





Hix::Features::Move::Move(GLModel* target, const QVector3D& to) : _model(target)
{
	_prevMatrix = target->transform().matrix();
	_prevAabb = target->aabb();
	target->moveModel(to);

	if(qmlManager->isActive<Hix::Features::WidgetMode>())
		qmlManager->cameraViewChanged();
}

Hix::Features::Move::Move(GLModel* target) : _model(target)
{
	_prevMatrix = target->transform().matrix();
	_prevAabb = target->aabb();
}

Hix::Features::Move::~Move()
{
}

void Hix::Features::Move::undo()
{
	_nextMatrix = _model->transform().matrix();
	_nextAabb = _model->aabb();

	_model->transform().setMatrix(_prevMatrix);
	_model->aabb() = _prevAabb;
	qmlManager->cameraViewChanged();
	_model->updatePrintable();
	_model->updateMesh();
}

void Hix::Features::Move::redo()
{
	_model->transform().setMatrix(_nextMatrix);
	_model->aabb() = _nextAabb;
	qmlManager->cameraViewChanged();
	_model->updatePrintable();
	_model->updateMesh();
}
