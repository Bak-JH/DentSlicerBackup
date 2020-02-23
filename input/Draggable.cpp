#include "Draggable.h"
#include "../../application/ApplicationManager.h"

Hix::Input::Draggable::~Draggable()
{
	Hix::Application::ApplicationManager::getInstance().getRayCaster().draggableObjectDeleted(this);
}
