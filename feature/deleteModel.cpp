#include "deleteModel.h"
#include "glmodel.h"
#include "qmlmanager.h"
#include "../application/ApplicationManager.h"
using namespace Hix::Application;
Hix::Features::DeleteModel::DeleteModel(GLModel* target):FlushSupport(target), _model(target)
{

}

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
	if (info.isListed)
	{
		qmlManager->addPart(raw->modelName(), raw->ID());
		qmlManager->addToGLModels(raw);
	}
}

void Hix::Features::DeleteModel::redoImpl()
{
	auto raw = std::get<GLModel*>(_model);
	auto parent = raw->parentNode();
	bool isListed = ApplicationManager::getInstance().partManager().isTopLevel(raw);
	raw->QNode::setParent((QNode*)nullptr);
	if (isListed)
	{
		qmlManager->unselectPart(raw);
		qmlManager->deletePartListItem(raw->ID());
		qmlManager->releaseFromGLModels(raw);
	}
	_model = RedoInfo{ std::unique_ptr<GLModel>(raw), parent, isListed };
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

Hix::Features::DeleteModelMode::DeleteModelMode()
{


	Hix::Application::ApplicationManager::getInstance().modalDialogManager().openOkCancelDialog(
		"Delete selected models?", "Ok", "Cancel", 
		[]() {
			//ok pressed
			auto selected = ApplicationManager::getInstance().partManager().selectedModels();
			Hix::Features::FeatureContainerFlushSupport* container = new Hix::Features::FeatureContainerFlushSupport(selected);
			for (auto& model : selected)
			{
				container->addFeature(new DeleteModel(model));
			}
			qmlManager->taskManager().enqueTask(container);
			qmlManager->setMode(nullptr);
		},
		[]() {
			qmlManager->setMode(nullptr);
		}
	);
}

Hix::Features::DeleteModelMode::~DeleteModelMode()
{
}
