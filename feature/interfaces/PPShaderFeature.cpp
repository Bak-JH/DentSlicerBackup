#include "PPShaderFeature.h"
#include "../../glmodel.h"
#include "../../qmlmanager.h"

Hix::Features::PPShaderFeature::PPShaderFeature(const std::unordered_set<GLModel*>& selectedModels)
	:_subjects(selectedModels), Mode()
{
	for (auto each : _subjects)
	{
		each->setMaterialMode(Hix::Render::ShaderMode::PerPrimitiveColor);
		each->updateMesh(true);

	}
}

void Hix::Features::PPShaderFeature::colorFaces(GLModel* model, const std::unordered_set<Hix::Engine3D::FaceConstItr>& faces)
{
	auto listed = model->getRootModel();
	listed->unselectMeshFaces();
	model->selectedFaces = faces;
	listed->updateMesh(true);
}

Hix::Features::PPShaderFeature::~PPShaderFeature()
{
	for (auto each : _subjects)
	{
		each->unselectMeshFaces();
		each->setMaterialMode(Hix::Render::ShaderMode::SingleColor);
		each->updatePrintable();
		//each->updateMesh(true);
	}
}
