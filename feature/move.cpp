#include "move.h"
#include "widget/MoveWidget.h"
#include "qmlmanager.h"
#include "application/ApplicationManager.h"

Hix::Features::MoveMode::MoveMode():WidgetMode(), _targetModels(Hix::Application::ApplicationManager::getInstance().partManager().selectedModels())
{
	_widget.addWidget(std::make_unique<Hix::UI::MoveWidget>(QVector3D(1, 0, 0), &_widget));
	_widget.addWidget(std::make_unique<Hix::UI::MoveWidget>(QVector3D(0, 1, 0), &_widget));
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

Hix::Features::FeatureContainerFlushSupport* Hix::Features::MoveMode::applyMove(const QVector3D& to)
{
	Hix::Features::FeatureContainerFlushSupport* container = new FeatureContainerFlushSupport(_targetModels);
	for (auto& target : _targetModels)
		container->addFeature(new Move(target, to));
	return container;
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
}

Hix::Features::Move::Move(GLModel* target) : _model(target)
{
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
