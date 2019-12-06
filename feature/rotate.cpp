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
	_rotateContainer = new FeatureContainer();
	for (auto& target : _targetModels)
		_rotateContainer->addFeature(new Rotate(target));

}

void Hix::Features::RotateMode::featureEnded()
{
	for (auto& each : _targetModels)
	{
		qmlManager->featureHistoryManager().addFeature(_rotateContainer);
		each->rotateDone();
		qmlManager->sendUpdateModelInfo();
	}
	_widget.updatePosition();
}

Hix::Features::Rotate::Rotate(GLModel* target) : _target(target)
{
	_prevMatrix = target->transform().matrix();
	_prevAabb = target->aabb();
}

Hix::Features::Rotate::~Rotate()
{

}

void Hix::Features::Rotate::undo()
{
	_target->transform().setMatrix(_prevMatrix);
	_target->aabb() = _prevAabb;
	qmlManager->cameraViewChanged();
}
