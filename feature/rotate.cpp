#include "rotate.h"
#include "widget/RotateWidget.h"
#include "application/ApplicationManager.h"
#include "../glmodel.h"
#include "../Qml/components/Inputs.h"
const QUrl ROTATE_POPUP_URL = QUrl("qrc:/Qml/FeaturePopup/PopupRotate.qml");
Hix::Features::RotateMode::RotateMode(const std::unordered_set<GLModel*>& models):
	_targetModels(models), DialogedMode(ROTATE_POPUP_URL)
{
	_widget.addWidget(std::make_unique<Hix::UI::RotateWidget>(QVector3D(1, 0, 0), &_widget, models));
	_widget.addWidget(std::make_unique<Hix::UI::RotateWidget>(QVector3D(0, 1, 0), &_widget, models));
	_widget.addWidget(std::make_unique<Hix::UI::RotateWidget>(QVector3D(0, 0, 1), &_widget, models));
	_widget.setVisible(true);
	auto& co = controlOwner();
	co.getControl(_xValue, "rotateX");
	co.getControl(_yValue, "rotateY");
	co.getControl(_zValue, "rotateZ");
	updatePosition();
}
Hix::Features::RotateMode::RotateMode(): RotateMode(Hix::Application::ApplicationManager::getInstance().partManager().selectedModels())
{
	if (Hix::Application::ApplicationManager::getInstance().partManager().selectedModels().empty())
	{
		Hix::Application::ApplicationManager::getInstance().modalDialogManager().needToSelectModels();
		return;
	}


}

Hix::Features::RotateMode::~RotateMode()
{

}

void Hix::Features::RotateMode::featureStarted()
{
	_rotateContainer = new FeatureContainerFlushSupport(_targetModels);
	for (auto& target : _targetModels)
		_rotateContainer->addFeature(new Rotate(target));
	if (!_rotateContainer->empty())
		Hix::Application::ApplicationManager::getInstance().taskManager().enqueTask(_rotateContainer);

}

void Hix::Features::RotateMode::featureEnded()
{
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
	auto rotation = QQuaternion::fromEulerAngles(_xValue->getValue(), _yValue->getValue(), _zValue->getValue());
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
	postUIthread([this]() {
		_nextMatrix = _model->transform().matrix();
		_nextAabb = _model->aabb();
		_model->transform().setMatrix(_prevMatrix);
		_model->aabb() = _prevAabb;
		UpdateWidgetModePos();
		_model->updateMesh();
		});

}

void Hix::Features::Rotate::redoImpl()
{
	postUIthread([this]() {
		_model->transform().setMatrix(_nextMatrix);
		_model->aabb() = _nextAabb;
		UpdateWidgetModePos();
		_model->updateMesh();
		});

}

void Hix::Features::Rotate::runImpl()
{
	postUIthread([this]() {
		_prevMatrix = _model->transform().matrix();
		_prevAabb = _model->aabb();
		if (_rot)
		{
			_model->rotateModel(_rot.value());
			_model->rotateDone();
			UpdateWidgetModePos();
		}
		});

}

const GLModel* Hix::Features::Rotate::model() const
{
	return _model;
}


