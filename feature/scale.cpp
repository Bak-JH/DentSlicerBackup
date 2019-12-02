#include "Scale.h"

Hix::Features::ScaleMode::ScaleMode(const std::unordered_set<GLModel*>& selectedModels)
	: _targetModels(selectedModels)
{
}

Hix::Features::ScaleMode::~ScaleMode()
{
}

std::unique_ptr<Hix::Features::FeatureContainer> Hix::Features::ScaleMode::applyScale(QVector3D scale)
{
	std::unique_ptr<Hix::Features::FeatureContainer> container = std::make_unique<FeatureContainer>();
	for (auto& target : _targetModels)
		container->addFeature(new Scale(target, scale));

	return container;
}

Hix::Features::Scale::Scale(GLModel* targetModel, QVector3D& scale)
	: _model(targetModel)
{
	_prevScale = targetModel->transform().scale3D() / scale;
	targetModel->scaleModel(scale);
	targetModel->scaleDone();
	qmlManager->sendUpdateModelInfo();
}

Hix::Features::Scale::~Scale()
{
}

void Hix::Features::Scale::undo()
{
	qDebug() << "scale: " << _prevScale;
	_model->scaleModel(_prevScale);
	_model->scaleDone();
	qmlManager->sendUpdateModelInfo();
}
