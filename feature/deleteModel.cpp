#include "deleteModel.h"
#include "glmodel.h"
#include "qmlmanager.h"

Hix::Features::DeleteModel::DeleteModel(GLModel* target)
{
	qmlManager->unselectPart(target);
	qmlManager->deletePartListItem(target->ID);
	qmlManager->supportRaftManager().clear(*target);

	_deletedModel = qmlManager->removeFromGLModels(target);
	qDebug() << _deletedModel->ID;

	_deletedModel->setHitTestable(false);
	_deletedModel->setEnabled(false);
	_deletedModel->QNode::setParent((QNode*)nullptr);
}

Hix::Features::DeleteModel::~DeleteModel()
{
}

void Hix::Features::DeleteModel::undo()
{
	qmlManager->addPart(_deletedModel->modelName(), _deletedModel->ID);
	_deletedModel->setEnabled(true);
	_deletedModel->setHitTestable(true);
	_deletedModel->QNode::setParent(qmlManager->models);
	qmlManager->addToGLModels(_deletedModel);
}

GLModel* Hix::Features::DeleteModel::getDeletedModel()
{
	return _deletedModel;
}
