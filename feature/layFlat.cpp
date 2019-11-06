#include "layFlat.h"
#include "qmlmanager.h"

Hix::Features::LayFlat::LayFlat(const std::unordered_set<GLModel*>& selectedModels):_models(selectedModels)
{
	for (auto each : _models)
	{
		each->setMaterialMode(Hix::Render::ShaderMode::PerPrimitiveColor);
		each->updateMesh(true);
	}
}

void Hix::Features::LayFlat::generateLayFlat()
{
	for (auto each : _models)
	{
		each->unselectMeshFaces();
		constexpr QVector4D worldBot(0, 0, -1, 1);
		QVector3D localBotNorml(each->toLocalCoord(worldBot));
		auto rotationTo = QQuaternion::rotationTo(each->targetMeshFace().localFn(), localBotNorml);
		each->transform().setRotation(each->transform().rotation() * rotationTo);
		qmlManager->resetLayflat();
	}
}
