#include <QFileDialog>
#include "../qmlmanager.h"
#include "../glmodel.h"
#include "addModel.h"
#include "repair/meshrepair.h"
#include "ImportModel.h"

Hix::Features::ImportModelMode::ImportModelMode()
{

	auto fileUrl = QFileDialog::getOpenFileUrl(nullptr, "Please choose a file", QUrl(), "3D files(*.stl *.obj)");
	if (!fileUrl.isEmpty())
	{
		qmlManager->taskManager().enqueTask(new ImportModel(fileUrl));
	}
	//QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));
}

Hix::Features::ImportModelMode::~ImportModelMode()
{
}

Hix::Features::ImportModel::ImportModel(QUrl fileUrl): _fileUrl(fileUrl)
{}

Hix::Features::ImportModel::~ImportModel()
{
}

void Hix::Features::ImportModel::undoImpl()
{
	tryUndoFeature(*_listModelFeature);
}

void Hix::Features::ImportModel::redoImpl()
{
	tryRedoFeature(*_listModelFeature);
}

void Hix::Features::ImportModel::runImpl()
{
	auto mesh = new Mesh();
	auto filename = _fileUrl.fileName();

	if (filename.contains(".stl") || filename.contains(".STL")) {
		FileLoader::loadMeshSTL(mesh, _fileUrl);
	}
	else if (filename.contains(".obj") || filename.contains(".OBJ")) {
		FileLoader::loadMeshOBJ(mesh, _fileUrl);
	}
	GLModel::filenameToModelName(filename.toStdString());
	mesh->centerMesh();

	_listModelFeature.reset(new ListModel(mesh, filename, nullptr));
	tryRunFeature(*_listModelFeature);

	//repair mode
	if (Hix::Features::isRepairNeeded(mesh))
	{
		//qmlManager->setProgressText("Repairing mesh.");
		std::unordered_set<GLModel*> repairModels;
		repairModels.insert(_listModelFeature->getAddedModel());
		MeshRepair repair(repairModels);
		repair.run();
	}
	// do auto arrange
	qmlManager->openArrange();
}
