#include "Hoverable.h"
#include "../../application/ApplicationManager.h"

//because we need to remove this hoverable pointer from raycaster when deleting
Hix::Input::Hoverable::~Hoverable()
{
	Hix::Application::ApplicationManager::getInstance().getRayCaster().hoverObjectDeleted(this);
}
