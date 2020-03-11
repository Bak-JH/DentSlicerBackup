#include "ImportModel.h"
#include <QFileDialog>
#include "../glmodel.h"
#include "addModel.h"
#include "repair/meshrepair.h"
#include "arrange/autoarrange.h"
#include "../application/ApplicationManager.h"

Hix::Features::ImportModelMode::ImportModelMode()
{

	auto fileUrl = QFileDialog::getOpenFileUrl(nullptr, "Please choose a file", QUrl(), "3D files(*.stl *.obj)");
	if (!fileUrl.isEmpty())
	{
		Hix::Application::ApplicationManager::getInstance().taskManager().enqueTask(new ImportModel(fileUrl));
	}
	//QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));
}

Hix::Features::ImportModelMode::~ImportModelMode()
{
}



Hix::Features::ImportModel::ImportModel(QUrl fileUrl): _fileUrl(fileUrl)
{
	_progress.setDisplayText("Import Model");
}

Hix::Features::ImportModel::~ImportModel()
{
}

GLModel* Hix::Features::ImportModel::get()
{
	for (auto& f : _container)
	{
		auto listModel = dynamic_cast<ListModel*>(f.get());
		if (listModel)
		{
			return listModel->get();
		}
	}
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

	auto listModel = new ListModel(mesh, filename, nullptr);
	tryRunFeature(*listModel);
	addFeature(listModel);
	//repair mode
	if (Hix::Features::isRepairNeeded(mesh))
	{
		//Hix::Application::ApplicationManager::getInstance().setProgressText("Repairing mesh.");
		auto repair = new MeshRepair(listModel->get());
		tryRunFeature(*repair);
		addFeature(repair);
	}
	auto arrange = new AutoArrangeAppend(listModel->get());
	tryRunFeature(*arrange);
	addFeature(arrange);

}
