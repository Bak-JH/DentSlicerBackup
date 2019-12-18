#include "addModel.h"
#include "qmlmanager.h"
#include "feature/deleteModel.h"

Hix::Features::AddModel::AddModel(Qt3DCore::QEntity* parent, Hix::Engine3D::Mesh* mesh, QString fname, int id, const Qt3DCore::QTransform* transform)
{
	qDebug() << "add " << id;
	_addedModel = new GLModel(parent, mesh, fname, id, transform);
	_addedModel->setHitTestable(true);
	qmlManager->addPart(fname, id);
	_addedModel->setZToBed();
}

Hix::Features::AddModel::~AddModel()
{
}

void Hix::Features::AddModel::undo()
{
	_deletedModel = _addedModel;
	qmlManager->deletePartListItem(_addedModel->ID);
	qmlManager->unselectPart(_addedModel);
	//if selected, remove from selected list
	qmlManager->supportRaftManager().clear(*_addedModel);

	_addedModel->QNode::setParent((QNode*)nullptr);
	qmlManager->removeFromGLModels(_addedModel);
}

void Hix::Features::AddModel::redo()
{
	_deletedModel->setParent(qmlManager->models);
	qmlManager->addPart(_deletedModel->modelName(), _deletedModel->ID);
	qmlManager->addToGLModels(_deletedModel);
}

GLModel* Hix::Features::AddModel::getAddedModel()
{
	return _addedModel;
}

std::unique_ptr<GLModel> Hix::Features::AddModel::getAddedModelUnique()
{
	return std::unique_ptr<GLModel>(_addedModel);
}
