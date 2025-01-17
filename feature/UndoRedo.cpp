#include "UndoRedo.h"
#include "../application/ApplicationManager.h"

Hix::Features::Undo::Undo()
{
	_progress.setDisplayText("Undo");
}

void Hix::Features::Undo::run()
{
	try
	{
		Hix::Application::ApplicationManager::getInstance().featureManager().featureHistoryManager().undo();
	}
	catch (...)
	{
		qDebug() << "undo failed";
	}

}

Hix::Features::Undo::~Undo()
{
}

Hix::Features::Redo::Redo()
{
	_progress.setDisplayText("Redo");
}

void Hix::Features::Redo::run()
{
	try
	{
		Hix::Application::ApplicationManager::getInstance().featureManager().featureHistoryManager().redo();
	}
	catch (...)
	{
		qDebug() << "redo failed";
	}
}

Hix::Features::Redo::~Redo()
{
}
