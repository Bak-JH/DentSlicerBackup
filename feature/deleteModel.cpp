#include "deleteModel.h"
#include "glmodel.h"
#include "qmlmanager.h"

Hix::Features::DeleteModel::DeleteModel(GLModel* target) : _deletedModel(target)
{
	qmlManager->modelSelected(target->ID);
	qmlManager->deletePartListItem(target->ID);
	//if selected, remove from selected list
	qmlManager->supportRaftManager().clear(*target);

	_deletedModel->setEnabled(false);
	_deletedModel->QNode::setParent((QNode*)nullptr);

	qmlManager->removeSelected(target);
}

Hix::Features::DeleteModel::~DeleteModel()
{
}

void Hix::Features::DeleteModel::undo()
{
	qmlManager->addPart(_deletedModel->modelName(), _deletedModel->ID);
	_deletedModel->QNode::setParent(qmlManager->models);
	_deletedModel->setEnabled(true);
}

GLModel* Hix::Features::DeleteModel::getDeletedModel()
{
	return _deletedModel;
}
