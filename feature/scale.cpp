#include "scale.h"
#include "../qml/components/Inputs.h"
#include "../qml/components/ControlOwner.h"
#include "../glmodel.h"
#include "application/ApplicationManager.h"

const QUrl SCALE_POPUP_URL = QUrl("qrc:/Qml/FeaturePopup/PopupScale.qml");
Hix::Features::ScaleMode::ScaleMode(): _targetModels(Hix::Application::ApplicationManager::getInstance().partManager().selectedModels()), DialogedMode(SCALE_POPUP_URL)
{
	auto& co = controlOwner();
	co.getControl(_precentValue, "scaleValue");
	co.getControl(_xValue, "scaleX");
	co.getControl(_yValue, "scaleY");
	co.getControl(_zValue, "scaleZ");
}
Hix::Features::ScaleMode::~ScaleMode()
{
}

Hix::Features::FeatureContainerFlushSupport* Hix::Features::ScaleMode::applyScale(QVector3D scale)
{
	Hix::Features::FeatureContainerFlushSupport* container = new FeatureContainerFlushSupport(_targetModels);
	for (auto& target : _targetModels)
		container->addFeature(new Scale(target, scale));

	return container;
}

void Hix::Features::ScaleMode::apply()
{
	auto scale = QVector3D();
	Hix::Features::FeatureContainerFlushSupport* container = new FeatureContainerFlushSupport(_targetModels);
	for (auto& target : _targetModels)
		container->addFeature(new Scale(target, scale));
}





Hix::Features::Scale::Scale(GLModel* targetModel, QVector3D& scale)
	: _model(targetModel), _scale(scale)
{
	_progress.setDisplayText("Scale Model");
}

Hix::Features::Scale::~Scale()
{
	_model = nullptr;
	delete _model;
}

void Hix::Features::Scale::undoImpl()
{
	_nextMatrix = _model->transform().matrix();
	_nextAabb = _model->aabb();

	_model->transform().setMatrix(_prevMatrix);
	_model->aabb() = _prevAabb;
	_model->updateMesh();
}

void Hix::Features::Scale::redoImpl()
{
	_model->transform().setMatrix(_nextMatrix);
	_model->aabb() = _nextAabb;
	_model->updateMesh();
}

void Hix::Features::Scale::runImpl()
{
	_prevMatrix = _model->transform().matrix();
	_prevAabb = _model->aabb();
	_model->scaleModel(_scale);
	_model->scaleDone();
}
