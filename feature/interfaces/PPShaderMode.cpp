#include "PPShaderMode.h"
#include "../../glmodel.h"


Hix::Features::PPShaderMode::PPShaderMode()
{
}

Hix::Features::PPShaderMode::PPShaderMode(const std::unordered_set<GLModel*>& selectedModels)
	:_subjects(selectedModels), Mode()
{
	for (auto each : _subjects)
	{
		initPPShader(*each);
	}
}

void Hix::Features::PPShaderMode::initPPShader(GLModel& model)
{
	model.setMaterialMode(Hix::Render::ShaderMode::PerPrimitiveColor);
	model.updateMesh(true);
}

void Hix::Features::PPShaderMode::colorFaces(GLModel* model, const std::unordered_set<Hix::Engine3D::FaceConstItr>& faces)
{
	auto listed = model->getRootModel();
	listed->unselectMeshFaces();
	model->selectedFaces = faces;
	listed->updateMesh(true);
}

Hix::Features::PPShaderMode::~PPShaderMode()
{
	for (auto each : _subjects)
	{
		each->unselectMeshFaces();
		each->setMaterialMode(Hix::Render::ShaderMode::SingleColor);
		each->updatePrintable();
		each->updateMesh(true);
	}
}
