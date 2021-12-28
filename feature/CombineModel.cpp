#include "CombineModel.h"
#include "glmodel.h"
#include "application/ApplicationManager.h"
#include "feature/addModel.h"

#include <QString>

using namespace Hix::Application;

Hix::Features::CombineModels::CombineModels(std::unordered_set<GLModel*>& targets, std::string groupName, bool isDisplay)
	: FlushSupport(targets), _targetModels(targets), _groupName(groupName), _isDisplay(isDisplay)
{
	_shellModel.reset(new GLModel(nullptr, nullptr, groupName.c_str()));
}

Hix::Features::CombineModels::~CombineModels()
{
}

void Hix::Features::CombineModels::undoImpl()
{
	for (auto model : _targetModels)
	{
		postUIthread([this, model]()
			{
				auto& partManager = ApplicationManager::getInstance().partManager();
				partManager.addPart(std::unique_ptr<GLModel>(model));
			});
	}
}

void Hix::Features::CombineModels::redoImpl()
{
}

void Hix::Features::CombineModels::runImpl()
{
	for (GLModel* model : _targetModels)
	{
		std::unordered_set<const GLModel*> sss;
		model->getChildrenModels(sss);
		qDebug() << model->modelName() << sss.size();

		postUIthread([this, model]()
			{
				auto& partManager = ApplicationManager::getInstance().partManager();
				partManager.removePart(model);
			});
	}

	//auto list = new ListModel(nullptr, QString(_groupName.c_str()), nullptr);
	//tryRunFeature(*list);
}

Hix::Features::CombineMode::CombineMode(std::unordered_set<GLModel*>& targets, std::string groupName, bool isDisplay)
{
	auto work = new CombineModels(targets, groupName, isDisplay);
	Hix::Application::ApplicationManager::getInstance().taskManager().enqueTask(work);
}

Hix::Features::CombineMode::~CombineMode()
{
}
