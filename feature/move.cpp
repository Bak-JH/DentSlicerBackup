#include "move.h"
#include "widget/MoveWidget.h"
#include "../qml/components/ControlOwner.h"
#include "../qml/components/Inputs.h"
#include "qmlmanager.h"
#include "application/ApplicationManager.h"

const QUrl MOVE_POPUP_URL = QUrl("qrc:/Qml/FeaturePopup/PopupMove.qml");
Hix::Features::MoveMode::MoveMode() : WidgetMode(), _targetModels(Hix::Application::ApplicationManager::getInstance().partManager().selectedModels()), DialogedMode(MOVE_POPUP_URL)
{
	_widget.addWidget(std::make_unique<Hix::UI::MoveWidget>(QVector3D(1, 0, 0), &_widget));
	_widget.addWidget(std::make_unique<Hix::UI::MoveWidget>(QVector3D(0, 1, 0), &_widget));

	auto& co = controlOwner();
	co.getControl(_xValue, "moveX");
	co.getControl(_yValue, "moveY");
	co.getControl(_zValue, "moveZ");
}

Hix::Features::MoveMode::~MoveMode()
{
}

void Hix::Features::MoveMode::featureStarted()
{
	_moveContainer = new FeatureContainerFlushSupport(_targetModels);
	for (auto& target : _targetModels)
		_moveContainer->addFeature(new Move(target));
}

void Hix::Features::MoveMode::featureEnded()
{
	if(!_moveContainer->empty())
		qmlManager->taskManager().enqueTask(_moveContainer);

	for (auto& each : _targetModels)
		each->moveDone();
	updatePosition();
}

void Hix::Features::MoveMode::apply()
{
	auto to = QVector3D(_xValue->getValue(), _yValue->getValue(), _zValue->getValue());

	Hix::Features::FeatureContainerFlushSupport* container = new FeatureContainerFlushSupport(_targetModels);
	for (auto& target : _targetModels)
		container->addFeature(new Move(target, to));

	qmlManager->taskManager().enqueTask(container);
}


QVector3D Hix::Features::MoveMode::getWidgetPosition()
{
	return 	Hix::Engine3D::combineBounds(_targetModels).centre();

}


void Hix::Features::MoveMode::modelMoveWithAxis(QVector3D axis, double distance) { // for QML Signal -> float is not working in qml signal parameter
	auto displacement = distance * axis;
	modelMove(displacement);
}

void Hix::Features::MoveMode::modelMove(QVector3D displacement)
{
	QVector3D bndCheckedDisp;
	const auto& printBound = qmlManager->settings().printerSetting.bedBound;
	for (auto selectedModel : _targetModels) {
		bndCheckedDisp = printBound.displaceWithin(selectedModel->recursiveAabb(), displacement);
		selectedModel->moveModel(bndCheckedDisp);
	}
}


Hix::Features::Move::Move(GLModel* target, const QVector3D& to) : _model(target), _to(to)
{
	_progress.setDisplayText("Move Model");
}

Hix::Features::Move::Move(GLModel* target) : _model(target)
{
	_progress.setDisplayText("Move Model");
}

Hix::Features::Move::~Move()
{
	_model = nullptr;
	delete _model;
}

void Hix::Features::Move::undoImpl()
{
	_nextMatrix = _model->transform().matrix();
	_nextAabb = _model->aabb();

	_model->transform().setMatrix(_prevMatrix);
	_model->aabb() = _prevAabb;
	qmlManager->cameraViewChanged();
	_model->updateMesh();
}

void Hix::Features::Move::redoImpl()
{
	_model->transform().setMatrix(_nextMatrix);
	_model->aabb() = _nextAabb;
	qmlManager->cameraViewChanged();
	_model->updateMesh();
}

void Hix::Features::Move::runImpl()
{
	_prevMatrix = _model->transform().matrix();
	_prevAabb = _model->aabb();
	if (_to)
	{
		_model->moveModel(_to.value());
		if (qmlManager->isActive<Hix::Features::WidgetMode>())
			qmlManager->cameraViewChanged();
	}
}

Hix::Features::Move::ZToBed::ZToBed(GLModel* target): Move(target)
{
	auto listedModel = target->getRootModel();
	_to = QVector3D(0, 0, -listedModel->recursiveAabb().zMin());
}
