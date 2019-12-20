#include "addModel.h"
#include "qmlmanager.h"
#include "feature/deleteModel.h"

Hix::Features::AddModel::AddModel(Qt3DCore::QEntity* parent, Hix::Engine3D::Mesh* mesh, QString fname, int id, const Qt3DCore::QTransform* transform)
{
	qDebug() << "add " << id;
	_addedModel = std::make_unique<GLModel>(parent, mesh, fname, id, transform);
	_addedModel->setHitTestable(true);
	qmlManager->addPart(fname, id);
	_addedModel->setZToBed();
	_deletedModel.reset(_addedModel.get());
}

Hix::Features::AddModel::~AddModel()
{
	_addedModel.release();
	_deletedModel.release();
}

void Hix::Features::AddModel::undo()
{
	qmlManager->deletePartListItem(_addedModel->ID);
	qmlManager->unselectPart(_addedModel.get());
	//if selected, remove from selected list
	//qmlManager->supportRaftManager().clear(*_addedModel);

	_addedModel->QNode::setParent((QNode*)nullptr);
	qmlManager->removeFromGLModels(_addedModel.get());
}

void Hix::Features::AddModel::redo()
{
	_deletedModel->setParent(qmlManager->models);
	qmlManager->addPart(_deletedModel->modelName(), _deletedModel->ID);
	qmlManager->addToGLModels(_deletedModel.get());
}

GLModel* Hix::Features::AddModel::getAddedModel()
{
	return _addedModel.get();
}

std::unique_ptr<GLModel> Hix::Features::AddModel::getAddedModelUnique()
{
	return std::move(_addedModel);
}
