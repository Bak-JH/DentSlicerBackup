#include "addModel.h"
#include "qmlmanager.h"




Hix::Features::AddModel::AddModel(Qt3DCore::QEntity* parent, Hix::Engine3D::Mesh* mesh, QString fname, const Qt3DCore::QTransform* transform)
{
	auto model = new GLModel(parent, mesh, fname, transform);
	model->setHitTestable(true);
	model->setZToBed();
	_model = model;
}

Hix::Features::AddModel::~AddModel()
{

}

void Hix::Features::AddModel::undo()
{
	auto raw = std::get<GLModel*>(_model);
	GLModel* parent = dynamic_cast<GLModel*>(raw->parentNode());
	raw->QNode::setParent((QNode*)nullptr);
	_model = UndoInfo{ std::unique_ptr<GLModel>(raw), parent };
}

void Hix::Features::AddModel::redo()
{
	auto& undoInfo = std::get<UndoInfo>(_model);
	auto& model = undoInfo.undoModel;
	model->setParent(undoInfo.parent);

}

GLModel* Hix::Features::AddModel::getAddedModel()
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
		return info.undoModel.get();
	}
}


Hix::Features::ListModel::ListModel(Hix::Engine3D::Mesh* mesh, QString fname, const Qt3DCore::QTransform* transform) : AddModel(qmlManager->models, mesh, fname, transform)
{
	auto rawModel = std::get<GLModel*>(_model);
	qmlManager->addToGLModels(rawModel);
	qmlManager->addPart(fname, rawModel->ID());

}

Hix::Features::ListModel::~ListModel()
{
}

void Hix::Features::ListModel::undo()
{
	auto raw = std::get<GLModel*>(_model);
	GLModel* parent = dynamic_cast<GLModel*>(raw->parentNode());
	raw->QNode::setParent((QNode*)nullptr);
	_model = UndoInfo{ qmlManager->removeFromGLModels(raw), parent };//parent is not glmodel
	qmlManager->deletePartListItem(raw->ID());
	qmlManager->unselectPart(raw);

}

void Hix::Features::ListModel::redo()
{
	auto& undoInfo = std::get<UndoInfo>(_model);
	auto& model = undoInfo.undoModel;
	model->setParent(undoInfo.parent);
	qmlManager->addPart(model->modelName(), model->ID());
	auto raw = model.get();
	qmlManager->addToGLModels(std::move(undoInfo.undoModel));
	_model = raw;
}
