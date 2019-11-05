#include "layFlat.h"
#include "qmlmanager.h"

Hix::Features::LayFlat::LayFlat(const std::unordered_set<GLModel*>& selectedModels)
	:_models(selectedModels)
{

}

void Hix::Features::LayFlat::generateLayFlat()
{
	for (auto selectedModel : _models)
	{
		if (!selectedModel->targetSelected())
			return;
		selectedModel->unselectMeshFaces();
		constexpr QVector4D worldBot(0, 0, -1, 1);
		QVector3D localBotNorml(selectedModel->toLocalCoord(worldBot));
		auto rotationTo = QQuaternion::rotationTo(selectedModel->targetMeshFace().localFn(), localBotNorml);
		selectedModel->transform().setRotation(selectedModel->transform().rotation() * rotationTo);
		qmlManager->resetLayflat();
	}
}
