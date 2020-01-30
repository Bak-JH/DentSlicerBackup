#include "UndoRedo.h"
#include "../qmlmanager.h"
Hix::Features::Undo::Undo()
{
}

void Hix::Features::Undo::run()
{
	try
	{
		qmlManager->featureHistoryManager().undo();
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
}

void Hix::Features::Redo::run()
{
	try
	{
		qmlManager->featureHistoryManager().redo();
	}
	catch (...)
	{
		qDebug() << "redo failed";
	}
}

Hix::Features::Redo::~Redo()
{
}
