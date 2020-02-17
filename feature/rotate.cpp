#include "rotate.h"
#include "widget/RotateWidget.h"
#include "application/ApplicationManager.h"
#include "qmlmanager.h"
#include "../Qml/components/Inputs.h"
const QUrl ROTATE_POPUP_URL = QUrl("qrc:/Qml/FeaturePopup/PopupRotate.qml");
Hix::Features::RotateMode::RotateMode(): WidgetMode(), _targetModels(Hix::Application::ApplicationManager::getInstance().partManager().selectedModels()), DialogedMode(ROTATE_POPUP_URL)
{
	_widget.addWidget(std::make_unique<Hix::UI::RotateWidget>(QVector3D(1, 0, 0), &_widget));
	_widget.addWidget(std::make_unique<Hix::UI::RotateWidget>(QVector3D(0, 1, 0), &_widget));
	_widget.addWidget(std::make_unique<Hix::UI::RotateWidget>(QVector3D(0, 0, 1), &_widget));

	auto& co = controlOwner();
	co.getControl(_xValue, "rotateX");
	co.getControl(_yValue, "rotateY");
	co.getControl(_zValue, "rotateZ");
}

Hix::Features::RotateMode::~RotateMode()
{

}

void Hix::Features::RotateMode::featureStarted()
{
	_rotateContainer = new FeatureContainerFlushSupport(_targetModels);
	for (auto& target : _targetModels)
		_rotateContainer->addFeature(new Rotate(target));

}

void Hix::Features::RotateMode::featureEnded()
{
	if (!_rotateContainer->empty())
		qmlManager->taskManager().enqueTask(_rotateContainer);
	for (auto& each : _targetModels)
	{
		each->rotateDone();
	}
	updatePosition();
}

QVector3D Hix::Features::RotateMode::getWidgetPosition()
{
	return 	Hix::Engine3D::combineBounds(_targetModels).centre();

}

std::unordered_set<GLModel*>& Hix::Features::RotateMode::models()
{
	return _targetModels;
}

Hix::Features::FeatureContainerFlushSupport* Hix::Features::RotateMode::applyRotate(const QQuaternion& rot)
{
	Hix::Features::FeatureContainerFlushSupport* container = new FeatureContainerFlushSupport(_targetModels);
	for (auto& target : _targetModels)
		container->addFeature(new Rotate(target, rot));
	return container;
}

Hix::Features::Rotate::Rotate(GLModel* target) : _model(target)
{}

Hix::Features::Rotate::Rotate(GLModel* target, const QQuaternion& rot) : _model(target), _rot(rot)
{

}

Hix::Features::Rotate::~Rotate()
{
	_model = nullptr;
	delete _model;
}

void Hix::Features::Rotate::undoImpl()
{
	_nextMatrix = _model->transform().matrix();
	_nextAabb = _model->aabb();

	_model->transform().setMatrix(_prevMatrix);
	_model->aabb() = _prevAabb;
	qmlManager->cameraViewChanged();
	_model->updateMesh();
}

void Hix::Features::Rotate::redoImpl()
{
	_model->transform().setMatrix(_nextMatrix);
	_model->aabb() = _nextAabb;
	qmlManager->cameraViewChanged();
	_model->updateMesh();
}

void Hix::Features::Rotate::runImpl()
{
	_prevMatrix = _model->transform().matrix();
	_prevAabb = _model->aabb();
	if (_rot)
	{
		_model->rotateModel(_rot.value());
		if (qmlManager->isActive<Hix::Features::WidgetMode>())
			qmlManager->cameraViewChanged();
	}
}

const GLModel* Hix::Features::Rotate::model() const
{
	return _model;
}

Hix::Features::RotateModeNoUndo::RotateModeNoUndo(const std::unordered_set<GLModel*>& targetModels):RotateMode()
{
	_targetModels = targetModels;
}

Hix::Features::RotateModeNoUndo::~RotateModeNoUndo()
{
}

void Hix::Features::RotateModeNoUndo::featureStarted()
{
}

void Hix::Features::RotateModeNoUndo::featureEnded()
{
	for (auto& each : _targetModels)
	{
		each->updateRecursiveAabb();
	}
	updatePosition();
}

