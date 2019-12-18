#include "deleteModel.h"
#include "glmodel.h"
#include "qmlmanager.h"

Hix::Features::DeleteModel::DeleteModel(GLModel* target)
{
	qmlManager->unselectPart(target);
	qmlManager->deletePartListItem(target->ID);
	_deletedModel = std::make_pair(qmlManager->removeFromGLModels(target), target->QNode::parent());

	target->QNode::setParent((QNode*)nullptr);
}

Hix::Features::DeleteModel::~DeleteModel()
{
}

void Hix::Features::DeleteModel::undo()
{
	_addedModel = _deletedModel;
	_deletedModel.first->QNode::setParent((QNode*)_deletedModel.second);

	if (!dynamic_cast<GLModel*>(_deletedModel.second))
	{
		qmlManager->addPart(_deletedModel.first->modelName(), _deletedModel.first->ID);
		qmlManager->addToGLModels(_deletedModel.first);
	}
}

void Hix::Features::DeleteModel::redo()
{
	qmlManager->unselectPart(_addedModel.first);
	qmlManager->deletePartListItem(_addedModel.first->ID);
	qmlManager->supportRaftManager().clear(*_addedModel.first);

	_deletedModel.first = qmlManager->removeFromGLModels(_addedModel.first);

	_addedModel.first->QNode::setParent((QNode*)nullptr);

}

GLModel* Hix::Features::DeleteModel::getDeletedModel()
{
	return _deletedModel.first;
}
