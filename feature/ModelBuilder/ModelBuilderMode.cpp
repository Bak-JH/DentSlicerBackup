#include "ModelBuilderMode.h"
#include <QFileDialog>
#include "../../qmlmanager.h"
#include "../../glmodel.h"
#include "../addModel.h"

constexpr float ZMARGIN = 5;
Hix::Features::ModelBuilderMode::ModelBuilderMode(): _topPlane(qmlManager->total, true), _bottPlane(qmlManager->total, true)
{
	QString fileName = QFileDialog::getOpenFileName(nullptr, "Select scanned surface file", "", "3D Model file (*.stl)");


	qmlManager->openProgressPopUp();
	auto mesh = new Mesh();
	if (fileName != "" && (fileName.contains(".stl") || fileName.contains(".STL"))) {
		FileLoader::loadMeshSTL(mesh, fileName.toLocal8Bit().constData());
	}
	else if (fileName != "" && (fileName.contains(".obj") || fileName.contains(".OBJ"))) {
		FileLoader::loadMeshOBJ(mesh, fileName.toLocal8Bit().constData());
	}
	fileName = qmlManager->filenameToModelName(fileName.toStdString());
	qmlManager->setProgress(0.1);
	_model.reset(new GLModel(qmlManager->models, mesh, fileName, nullptr));
	_model->setHitTestable(true);
	_model->setZToBed();
	_model->moveModel(QVector3D(0, 0, ZMARGIN));
	_topPlane.transform().setTranslation(QVector3D(0, 0, _model->aabb().zMax()));
	_bottPlane.transform().setTranslation(QVector3D(0, 0, _model->aabb().zMin()));

	//auto addModel = new ListModel(mesh, fileName, nullptr);
	//qmlManager->featureHistoryManager().addFeature(addModel);
	//auto latest = addModel->getAddedModel();

	//qmlManager->setProgress(0.2);
	//TwoManifoldBuilder modelBuilder(*mesh);
	//mesh->centerMesh();
	//qmlManager->setProgress(0.4);
	//repair mode
	//if (Hix::Features::isRepairNeeded(mesh))
	//{
	//	qmlManager->setProgressText("Repairing mesh.");
	//	std::unordered_set<GLModel*> repairModels;
	//	repairModels.insert(latest);
	//	MeshRepair sfsdfs(repairModels);
	//}
	//setProgress(1.0);
	//// do auto arrange
	//if (glmodels.size() >= 2)
	//	openArrange();
	////runArrange();
}

Hix::Features::ModelBuilderMode::~ModelBuilderMode()
{
}
