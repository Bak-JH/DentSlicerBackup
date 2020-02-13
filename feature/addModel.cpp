#include "addModel.h"
#include "qmlmanager.h"




Hix::Features::AddModel::AddModel(Qt3DCore::QEntity* parent, Hix::Engine3D::Mesh* mesh, QString fname, const Qt3DCore::QTransform* transform): 
	_parent(parent), _mesh(mesh), _fname(fname), _transform(transform)
{}

Hix::Features::AddModel::~AddModel()	
{
}

void Hix::Features::AddModel::undoImpl()
{
	std::function<UndoInfo()> undo = [this]()->UndoInfo
	{
		auto raw = std::get<GLModel*>(_model);
		auto parent = raw->parentNode();
		raw->QNode::setParent((Qt3DCore::QNode*)nullptr);
		return UndoInfo{ std::unique_ptr<GLModel>(raw), parent };
	};
	_model = postUIthread(std::move(undo));
}

void Hix::Features::AddModel::redoImpl()
{
	std::function<GLModel*()> redo = [this]()->GLModel*
	{
		auto& undoInfo = std::get<UndoInfo>(_model);
		auto& model = undoInfo.undoModel;
		model->setParent(undoInfo.parent);
		return model.release();
	};
	
	_model = postUIthread(std::move(redo));
}

void Hix::Features::AddModel::runImpl()
{
	std::function<GLModel*()> createModel = [this]()->GLModel*
	{
		auto model = new GLModel(_parent, _mesh, _fname, _transform);
		model->setHitTestable(true);
		model->setZToBed();
		return model;
	};
	_model = postUIthread(std::move(createModel));
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


Hix::Features::ListModel::ListModel(Hix::Engine3D::Mesh* mesh, QString fname, const Qt3DCore::QTransform* transform) : AddModel(qmlManager->total, mesh, fname, transform)
{}

Hix::Features::ListModel::~ListModel()
{
}

void Hix::Features::ListModel::undoImpl()
{
	AddModel::undoImpl();

	std::function<void()> undo = [this]()
	{
		auto raw = std::get<UndoInfo>(_model).undoModel.get();
		qmlManager->deletePartListItem(raw->ID());
		qmlManager->unselectPart(raw);
		qmlManager->releaseFromGLModels(raw);
	};
	postUIthread(std::move(undo));
}

void Hix::Features::ListModel::redoImpl()
{
	AddModel::redoImpl();

	std::function<void()> redo = [this]()
	{
		auto raw = std::get<GLModel*>(_model);
		qmlManager->addPart(raw->modelName(), raw->ID());
		qmlManager->addToGLModels(raw);
	};
	postUIthread(std::move(redo));
}

void Hix::Features::ListModel::runImpl()
{
	AddModel::runImpl();

	std::function<void()> redo = [this]()
	{
		auto rawModel = std::get<GLModel*>(_model);
		qmlManager->addToGLModels(rawModel);
		qmlManager->addPart(_fname, rawModel->ID());
	};
	QMetaObject::invokeMethod(qmlManager, redo, Qt::BlockingQueuedConnection);
}
