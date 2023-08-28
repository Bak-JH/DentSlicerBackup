#include "KeyboardController.h"
#include <QNode>

#include "../feature/UndoRedo.h"
#include "../feature/FeaturesLoader.h"
#include "../application/ApplicationManager.h"
#include "../Qml/util/QMLUtil.h"
#include "../feature/CopyPaste.h"
#include "feature/stlexport.h"
#include "feature/move.h"
#include "feature/rotate.h"
#include "feature/scale.h"
using namespace Hix::Input;
using namespace Hix::Features;
using namespace Hix::Application;

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
		Hix::Application::ApplicationManager::getInstance().partManager().setContiguousSelect(true);
		break;
	}
	case Qt::Key_Control:
	{
		_ctrlPressed = true;
		Hix::Application::ApplicationManager::getInstance().partManager().setMultiSelect(true);
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
		Hix::Application::ApplicationManager::getInstance().partManager().setContiguousSelect(false);
		break;
	}
	case Qt::Key_Control:
	{
		_ctrlPressed = false;
		Hix::Application::ApplicationManager::getInstance().partManager().setMultiSelect(false);
		break;
	}
	case Qt::Key_W:
	{
		Hix::Application::ApplicationManager::getInstance().featureManager().setMode(new MoveMode());
		break;
	}
	case Qt::Key_E:
	{
		Hix::Application::ApplicationManager::getInstance().featureManager().setMode(new RotateMode());
		break;
	}
	case Qt::Key_R:
	{
		Hix::Application::ApplicationManager::getInstance().featureManager().setMode(new ScaleMode());
		break;
	}
	default:
		break;
	}

	if (e->matches(QKeySequence::SelectAll))
	{
		ApplicationManager::getInstance().partManager().selectAll();
	}
	else if (e->matches(QKeySequence::Undo))
	{
		ApplicationManager::getInstance().taskManager().enqueTask(std::make_unique<Hix::Features::Undo>());
	}
	else if (e->matches(QKeySequence::Redo))
	{
		ApplicationManager::getInstance().taskManager().enqueTask(std::make_unique<Hix::Features::Redo>());
	}
	else if (e->matches(QKeySequence::Copy))
	{
		auto& man = ApplicationManager::getInstance();
		//copy mode doesn't close other modes
		if (!man.featureManager().isFeatureActive())
		{
			_copyModels = man.partManager().selectedModels();
		}
	}
	else if (e->matches(QKeySequence::Paste))
	{
		auto& man = ApplicationManager::getInstance();
		if (!man.featureManager().isFeatureActive())
		{
			for (auto& m : _copyModels)
			{
				//the model might be deleted
				if (man.partManager().isTopLevel(m))
				{
					man.taskManager().enqueTask(new CopyPaste(m));
				}
			}
		}
	}
	else if (e->matches(QKeySequence::Cancel))
	{
		//doesn't work in current focus system ie) key ignored when mode is active
		Hix::Application::ApplicationManager::getInstance().featureManager().setMode(nullptr);
		Hix::Application::ApplicationManager::getInstance().modalDialogManager().closeDialog();
	}
	else if (e->matches(QKeySequence::Delete))
	{
		if(!Hix::Application::ApplicationManager::getInstance().partManager().selectedModels().empty())
			Hix::Application::ApplicationManager::getInstance().partManager().deleteSelectedModels();
	}
	else if (e->matches(QKeySequence::Save))
	{
		_ctrlPressed = false;
		Hix::Application::ApplicationManager::getInstance().partManager().setMultiSelect(false);
		Hix::Application::ApplicationManager::getInstance().featureManager().setMode(new Hix::Features::STLExportMode());
	}
}

Hix::Input::KeyboardController::~KeyboardController()
{

}

