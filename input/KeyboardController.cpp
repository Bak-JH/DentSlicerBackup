#include "KeyboardController.h"
#include <QNode>

#include "../feature/UndoRedo.h"
#include "../feature/FeaturesLoader.h"
#include "../application/ApplicationManager.h"
#include "../Qml/util/QMLUtil.h"
using namespace Hix::Input;


Hix::Input::KeyboardController::KeyboardController()
{

}

void Hix::Input::KeyboardController::keyPressed(QKeyEvent* e)
{
	switch (e->key())
	{
	case Qt::Key_Shift:
	{
		_shiftPressed = true;
		Hix::Application::ApplicationManager::getInstance().partManager().setMultiSelect(true);
		break;
	}
	case Qt::Key_Control:
	{
		_ctrlPressed = true;
		break;
	}
	default:
		break;
	}
}

void Hix::Input::KeyboardController::keyReleased(QKeyEvent* e)
{
	switch (e->key())
	{
	case Qt::Key_Shift:
	{
		_shiftPressed = false;
		Hix::Application::ApplicationManager::getInstance().partManager().setMultiSelect(false);
		break;
	}
	case Qt::Key_Control:
	{
		_ctrlPressed = false;
		break;
	}
	default:
		break;
	}

	if (e->matches(QKeySequence::SelectAll))
	{
		Hix::Application::ApplicationManager::getInstance().partManager().selectAll();
	}
	else if (e->matches(QKeySequence::Undo))
	{
		Hix::Application::ApplicationManager::getInstance().taskManager().enqueTask(new Hix::Features::Undo());
	}
	else if (e->matches(QKeySequence::Redo))
	{
		Hix::Application::ApplicationManager::getInstance().taskManager().enqueTask(new Hix::Features::Redo());
	}
	else if (e->matches(QKeySequence::Copy))
	{
		//TODO
	}
	else if (e->matches(QKeySequence::Paste))
	{
		//TODO
	}
	else if (e->matches(QKeySequence::Cancel))
	{
		//TODO
	}
	else if (e->matches(QKeySequence::Delete))
	{
		Hix::Application::ApplicationManager::getInstance().partManager().deleteSelectedModels();
	}
}

Hix::Input::KeyboardController::~KeyboardController()
{

}

