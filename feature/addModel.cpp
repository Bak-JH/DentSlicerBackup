#include "addModel.h"
#include "qmlmanager.h"
#include "feature/deleteModel.h"

Hix::Features::AddModel::AddModel(Qt3DCore::QEntity* parent, Hix::Engine3D::Mesh* mesh, QString fname, int id, const Qt3DCore::QTransform* transform)
{
	_addedModel = new GLModel(parent, mesh, fname, id, transform);
	_addedModel->setHitTestable(true);
	qmlManager->addPart(fname, id);
}

Hix::Features::AddModel::AddModel(Qt3DCore::QEntity* parent, GLModel* model, int id)
{
	Qt3DCore::QTransform toRoot;
	toRoot.setMatrix(model->toRootMatrix());
	_addedModel = new GLModel(parent, model->getMeshModd(), model->modelName(), id, &toRoot);
	_addedModel->setEnabled(true);
	_addedModel->setHitTestable(true);
	_addedModel->setParent(parent);
	qmlManager->addPart(_addedModel->modelName(), _addedModel->ID);
}

Hix::Features::AddModel::~AddModel()
{
}

void Hix::Features::AddModel::undo()
{
	qmlManager->deletePartListItem(_addedModel->ID);
	qmlManager->unselectPart(_addedModel);
	//if selected, remove from selected list
	qmlManager->supportRaftManager().clear(*_addedModel);

	_addedModel->QNode::setParent((QNode*)nullptr);
	_addedModel->setEnabled(false);
	qmlManager->removeFromGLModels(_addedModel);
}

void Hix::Features::AddModel::redo()
{

} 

GLModel* Hix::Features::AddModel::getAddedModel()
{
	return _addedModel;
}

std::unique_ptr<GLModel> Hix::Features::AddModel::getAddedModelUnique()
{
	return std::unique_ptr<GLModel>(_addedModel);
}
