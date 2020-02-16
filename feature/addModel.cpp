#include "addModel.h"
#include "qmlmanager.h"
#include "../application/ApplicationManager.h"

using namespace Hix::Features;
using namespace Hix::Application;
using namespace Qt3DCore;

Hix::Features::AddModel::AddModel(Qt3DCore::QEntity* parent, Hix::Engine3D::Mesh* mesh, QString fname, const Qt3DCore::QTransform* transform): 
	_parent(parent), _mesh(mesh), _fname(fname), _transform(transform)
{}

Hix::Features::AddModel::~AddModel()
{
}

void Hix::Features::AddModel::undoImpl()
{
	auto raw = std::get<GLModel*>(_model);
	auto parent = raw->parentNode();
	raw->QNode::setParent((QNode*)nullptr);
	_model = UndoInfo{ std::unique_ptr<GLModel>(raw), parent};
}

void Hix::Features::AddModel::redoImpl()
{
	auto& undoInfo = std::get<UndoInfo>(_model);
	auto& model = undoInfo.undoModel;
	_model = model.release();
	model->setParent(undoInfo.parent);
	
}

void Hix::Features::AddModel::runImpl()
{
	auto model = new GLModel(_parent, _mesh, _fname, _transform);
	model->setHitTestable(true);
	model->setZToBed();
	_model = model;
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


Hix::Features::ListModel::ListModel(Hix::Engine3D::Mesh* mesh, QString fname, const Qt3DCore::QTransform* transform) : 
	AddModel(ApplicationManager::getInstance().partManager().modelRoot(), mesh, fname, transform)
{}

Hix::Features::ListModel::~ListModel()
{
}

void Hix::Features::ListModel::undoImpl()
{
	AddModel::undoImpl();
	auto raw = std::get<UndoInfo>(_model).undoModel.get();
	auto& partManager = ApplicationManager::getInstance().partManager();
	auto owningPtr = partManager.removePart(raw);
	owningPtr.release(); //since unique_ptr ownership is stolen in AddModel::undoImpl
}

void Hix::Features::ListModel::redoImpl()
{
	AddModel::redoImpl();
	auto raw = std::get<GLModel*>(_model);
	auto& partManager = ApplicationManager::getInstance().partManager();
	partManager.addPart(std::unique_ptr<GLModel>(raw));
}

void Hix::Features::ListModel::runImpl()
{
	AddModel::runImpl();
	auto rawModel = std::get<GLModel*>(_model);
	auto& partManager = ApplicationManager::getInstance().partManager();
	partManager.addPart(std::unique_ptr<GLModel>(rawModel));
}
