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

Hix::Input::KeyboardController::~KeyboardController()
{

}

bool Hix::Input::KeyboardController::isMultiselect() const
{
	return _shiftPressed;
}

void Hix::Input::KeyboardController::initMouseEvents()
{
	QObject::connect(_keyboardHandler, &Qt3DInput::QKeyboardHandler::pressed, [this](Qt3DInput::QKeyEvent* e) {
		qDebug() << e->key();
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

		});
	QObject::connect(_keyboardHandler, &Qt3DInput::QKeyboardHandler::released, [this](Qt3DInput::QKeyEvent* e) {
		qDebug() << e->key() << "released";
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
		});
	//QObject::connect(_keyboardHandler, &Qt3DInput::QKeyboardHandler::focusChanged, [this](bool isFocus) {
	//	qDebug() << "focus" << isFocus;
	//	_keyboardHandler->setFocus(true);
	//	});
}

void Hix::Input::KeyboardControllerLoader::init(KeyboardController& manager, Qt3DCore::QEntity* parentEntity)
{
	//Hix::QML::getItemByID(parentEntity, manager._keyboardHandler, "keyboardHandler");
	//manager.initMouseEvents();


	//manager._keyboardHandler->setFocus(true);

}
