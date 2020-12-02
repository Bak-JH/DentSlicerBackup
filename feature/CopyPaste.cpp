#include "CopyPaste.h"
#include "../glmodel.h"
#include "arrange/autoarrange.h"
#include "../application/ApplicationManager.h"
#include "../glmodel.h"
using namespace Hix::Application;

Hix::Features::CopyPasteMode::CopyPasteMode(const std::unordered_set<GLModel*>& targets): _targets(targets)
{
}

Hix::Features::CopyPasteMode::~CopyPasteMode()
{
}

void Hix::Features::CopyPasteMode::apply()
{
	auto& man = Hix::Application::ApplicationManager().getInstance().taskManager();
	for (auto& t : _targets)
	{
		man.enqueTask(new CopyPaste(t));
	}
}


Hix::Features::CopyPaste::CopyPaste(GLModel* target): _target(target)
{
}

Hix::Features::CopyPaste::~CopyPaste()
{
}


void Hix::Features::CopyPaste::runImpl()
{
	postUIthread([this]()
		{
			_model = new GLModel(*_target); //copy constructor
			auto rawModel = std::get<GLModel*>(_model);
			auto& partManager = ApplicationManager::getInstance().partManager();
			partManager.addPart(std::unique_ptr<GLModel>(rawModel));
			std::get<GLModel*>(_model)->setHitTestable(true);
		});
	ApplicationManager::getInstance().taskManager().enqueTask(new AutoArrangeAppend(std::get<GLModel*>(_model)));
}

