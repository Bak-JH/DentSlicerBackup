#include "deleteModel.h"
#include "glmodel.h"

#include "../application/ApplicationManager.h"
using namespace Hix::Application;
using namespace Qt3DCore;
Hix::Features::DeleteModel::DeleteModel(GLModel* target):FlushSupport(target), _model(target)
{
	_progress.setDisplayText("Delete Model");
}

Hix::Features::DeleteModel::~DeleteModel()
{
	
}

void Hix::Features::DeleteModel::undoImpl()
{
	auto& info = std::get<RedoInfo>(_model);
	auto raw = info.redoModel.get();
	if (info.isListed)
	{
		auto& partManager = ApplicationManager::getInstance().partManager();
		partManager.addPart(std::move(info.redoModel));
	}
	else
	{
		info.redoModel.release();
	}
	raw->setParent(info.parent);
	_model = raw;
}

void Hix::Features::DeleteModel::redoImpl()
{
	auto raw = std::get<GLModel*>(_model);
	auto parent = raw->parentNode();
	bool isListed = ApplicationManager::getInstance().partManager().isTopLevel(raw);
	auto deleteModel = [&raw]() { raw->QNode::setParent((Qt3DCore::QNode*)nullptr); };
	postUIthread(deleteModel);
	if (isListed)
	{
		auto& partManager = ApplicationManager::getInstance().partManager();
		_model = RedoInfo{ partManager.removePart(raw), parent, isListed };
	}
	else
	{
		_model = RedoInfo{ std::unique_ptr<GLModel>(raw), parent, isListed };
	}
	
}

void Hix::Features::DeleteModel::runImpl()
{
	auto closeModal = []() { 
		Hix::Application::ApplicationManager::getInstance().modalDialogManager().closeDialog(); 
	};
	qDebug() << _progress.getDisplayText();
	redoImpl();
	postUIthread(closeModal);
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
			Hix::Application::ApplicationManager::getInstance().taskManager().enqueTask(container);
			Hix::Application::ApplicationManager::getInstance().featureManager().setMode(nullptr);
		},
		[]() {
			Hix::Application::ApplicationManager::getInstance().featureManager().setMode(nullptr);
		}
	);
}

Hix::Features::DeleteModelMode::~DeleteModelMode()
{
}
