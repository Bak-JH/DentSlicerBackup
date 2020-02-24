#include "KeyboardController.h"
#include <QNode>
#include <QKeyboardHandler>

#include "../feature/UndoRedo.h"
using namespace Hix::Input;


Hix::Input::KeyboardController::KeyboardController(Qt3DCore::QNode* parent): _keyboardHandler(new Qt3DInput::QKeyboardHandler(parent))
{
	QObject::connect(_keyboardHandler, &Qt3DInput::QKeyboardHandler::pressed, [this](Qt3DInput::QKeyEvent* e) {
		switch (e->key())
		{
		case Qt::Key_Shift:
		{
			_shiftPressed = true;
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
		if (e->matches(QKeySequence::SelectAll))
		{
			//Hix::Application::ApplicationManager::getInstance().selectAll();
		}
		else if(e->matches(QKeySequence::Undo))
		{

		}
		else if (e->matches(QKeySequence::Redo))
		{

		}
		else if (e->matches(QKeySequence::Copy))
		{

		}
		else if (e->matches(QKeySequence::Paste))
		{

		}
		else if (e->matches(QKeySequence::Cancel))
		{

		}
		});
	QObject::connect(_keyboardHandler, &Qt3DInput::QKeyboardHandler::released, [this](Qt3DInput::QKeyEvent* e) {
		switch (e->key())
		{
		case Qt::Key_Shift:
		{
			_shiftPressed = false;
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
		});
}

Hix::Input::KeyboardController::~KeyboardController()
{

}

bool Hix::Input::KeyboardController::isMultiselect() const
{
	return _shiftPressed;
}
