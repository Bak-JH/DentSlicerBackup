#include "rotate.h"
#include "widget/RotateWidget.h"
#include "application/ApplicationManager.h"
#include "../glmodel.h"
#include "../Qml/components/Inputs.h"
const QUrl ROTATE_POPUP_URL = QUrl("qrc:/Qml/FeaturePopup/PopupRotate.qml");
Hix::Features::RotateMode::RotateMode(): WidgetMode(), _targetModels(Hix::Application::ApplicationManager::getInstance().partManager().selectedModels()), DialogedMode(ROTATE_POPUP_URL)
{
	if (Hix::Application::ApplicationManager::getInstance().partManager().selectedModels().empty())
	{
		Hix::Application::ApplicationManager::getInstance().modalDialogManager().needToSelectModels();
	}
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
		Hix::Application::ApplicationManager::getInstance().taskManager().enqueTask(_rotateContainer);
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

void Hix::Features::RotateMode::applyButtonClicked()
{
	auto rotation = QQuaternion(QVector3D(_xValue->getValue(), _yValue->getValue(), _zValue->getValue()));
	Hix::Features::FeatureContainerFlushSupport* container = new FeatureContainerFlushSupport(_targetModels);

	for (auto& target : _targetModels)
		container->addFeature(new Rotate(target, rotation));

	Hix::Application::ApplicationManager::getInstance().taskManager().enqueTask(container);
}

Hix::Features::Rotate::Rotate(GLModel* target) : _model(target)
{
	_progress.setDisplayText("Rotate Model");
}

Hix::Features::Rotate::Rotate(GLModel* target, const QQuaternion& rot) : _model(target), _rot(rot)
{
	_progress.setDisplayText("Rotate Model");
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
	UpdateWidgetModePos();
	_model->updateMesh();
}

void Hix::Features::Rotate::redoImpl()
{
	_model->transform().setMatrix(_nextMatrix);
	_model->aabb() = _nextAabb;
	UpdateWidgetModePos();
	_model->updateMesh();
}

void Hix::Features::Rotate::runImpl()
{
	_prevMatrix = _model->transform().matrix();
	_prevAabb = _model->aabb();
	if (_rot)
	{
		_model->rotateModel(_rot.value());
		UpdateWidgetModePos();
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

