#include "deleteModel.h"
#include "glmodel.h"
#include "qmlmanager.h"

Hix::Features::DeleteModel::DeleteModel(GLModel* target):_model(target)
{}

Hix::Features::DeleteModel::~DeleteModel()
{
}

void Hix::Features::DeleteModel::undoImpl()
{
	auto& info = std::get<RedoInfo>(_model);
	auto& model = info.redoModel;
	auto raw = model.release();
	_model = raw;
	model->setParent(info.parent);
	if (info.parent == qmlManager->models)
	{
		qmlManager->addPart(raw->modelName(), raw->ID());
		qmlManager->addToGLModels(raw);
	}
}

void Hix::Features::DeleteModel::redoImpl()
{
	auto raw = std::get<GLModel*>(_model);
	auto parent = raw->parentNode();
	raw->QNode::setParent((Qt3DCore::QNode*)nullptr);
	if (parent == qmlManager->models)
	{
		qmlManager->unselectPart(raw);
		qmlManager->deletePartListItem(raw->ID());
		qmlManager->releaseFromGLModels(raw);
	}
	_model = RedoInfo{ std::unique_ptr<GLModel>(raw), parent };
}

void Hix::Features::DeleteModel::runImpl()
{
	redoImpl();
}

GLModel* Hix::Features::DeleteModel::getDeletedModel()
{
	auto idx = _model.index();
	if (idx == 0)
	{
		auto& ptr = std::get<0>(_model);
		return ptr;
	}
	else
	{
		auto& info = std::get<1>(_model);
		return info.redoModel.get();
	}
}
