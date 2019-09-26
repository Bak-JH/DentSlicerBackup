#include "Hoverable.h"
#include "../qmlmanager.h"
//because we need to remove this hoverable pointer from raycaster when deleting
Hix::Input::Hoverable::~Hoverable()
{
	qmlManager->getRayCaster().hoverObjectDeleted(this);
}
