#include "Draggable.h"
#include "../qmlmanager.h"
Hix::Input::Draggable::~Draggable()
{
	qmlManager->getRayCaster().draggableObjectDeleted(this);
}
