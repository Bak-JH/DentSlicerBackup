#include "move.h"
#include "widget/MoveWidget.h"
#include "../qml/components/ControlOwner.h"
#include "../qml/components/Inputs.h"
#include "../glmodel.h"
#include "application/ApplicationManager.h"

#include "../qml/components/Buttons.h"

const QUrl MOVE_POPUP_URL = QUrl("qrc:/Qml/FeaturePopup/PopupMove.qml");
Hix::Features::MoveMode::MoveMode() : WidgetMode()
	, _targetModels(Hix::Application::ApplicationManager::getInstance().partManager().selectedModels())
	, DialogedMode(MOVE_POPUP_URL)
{
	if (Hix::Application::ApplicationManager::getInstance().partManager().selectedModels().empty())
	{
		Hix::Application::ApplicationManager::getInstance().modalDialogManager().needToSelectModels();
		return;
	}
	_widget.addWidget(std::make_unique<Hix::UI::MoveWidget>(QVector3D(1, 0, 0), &_widget));
	_widget.addWidget(std::make_unique<Hix::UI::MoveWidget>(QVector3D(0, 1, 0), &_widget));
	_widget.setVisible(true);

	auto& co = controlOwner();
	co.getControl(_xValue, "moveX");
	co.getControl(_yValue, "moveY");
	co.getControl(_zValue, "moveZ");
	co.getControl(_snapButton, "snapButton");

	QObject::connect(_snapButton, &Hix::QML::Controls::Button::clicked, [this]() {
			Hix::Features::FeatureContainerFlushSupport* container = new FeatureContainerFlushSupport(_targetModels);
			for (auto& target : _targetModels)
				container->addFeature(new Move::ZToBed(target));
			Hix::Application::ApplicationManager::getInstance().taskManager().enqueTask(container);
		});

	updatePosition();
}

Hix::Features::MoveMode::~MoveMode()
{
}

void Hix::Features::MoveMode::featureStarted()
{
	_widget.setManipulated(true);
	_moveContainer = new FeatureContainerFlushSupport(_targetModels);
	for (auto& target : _targetModels)
		_moveContainer->addFeature(new Move(target));
	if (!_moveContainer->empty())
		Hix::Application::ApplicationManager::getInstance().taskManager().enqueTask(_moveContainer);
}

void Hix::Features::MoveMode::featureEnded()
{
	_widget.setManipulated(false);
	for (auto& each : _targetModels)
		each->moveDone();
	updatePosition();
}

void Hix::Features::MoveMode::applyButtonClicked()
{
	auto to = QVector3D(_xValue->getValue(), _yValue->getValue(), _zValue->getValue());
	if (to == QVector3D(0, 0, 0))
		return;

	Hix::Features::FeatureContainerFlushSupport* container = new FeatureContainerFlushSupport(_targetModels);
	const auto& printBound = Hix::Application::ApplicationManager::getInstance().settings().printerSetting.bedBound;
	for (auto& target : _targetModels)
	{
		auto checkedDisp = printBound.displaceWithin(target->recursiveAabb(), to);
		container->addFeature(new Move(target, checkedDisp));

	}

	container->progress()->setDisplayText("Move Model");
	Hix::Application::ApplicationManager::getInstance().taskManager().enqueTask(container);
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
	//updatePosition();
	QVector3D bndCheckedDisp;
	const auto& printBound = Hix::Application::ApplicationManager::getInstance().settings().printerSetting.bedBound;
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
}

void Hix::Features::Move::undoImpl()
{
	postUIthread([this]() {
		_nextMatrix = _model->transform().matrix();
		_nextAabb = _model->aabb();

		_model->transform().setMatrix(_prevMatrix);
		_model->aabb() = _prevAabb;
		UpdateWidgetModePos();
		//_model->updateMesh();
	});
}

void Hix::Features::Move::redoImpl()
{
	postUIthread([this]() {
		_model->transform().setMatrix(_nextMatrix);
		_model->aabb() = _nextAabb;
		UpdateWidgetModePos();
		//_model->updateMesh();
	});
}

void Hix::Features::Move::runImpl()
{
	postUIthread([this]() {
		_prevMatrix = _model->transform().matrix();
		_prevAabb = _model->aabb();
		if (_to)
		{
			_model->moveModel(_to.value());
			UpdateWidgetModePos();
		}
	});



}

Hix::Features::Move::ZToBed::ZToBed(GLModel* target): Move(target)
{
	auto listedModel = target->getRootModel();
	_to = QVector3D(0, 0, -listedModel->recursiveAabb().zMin());
}
