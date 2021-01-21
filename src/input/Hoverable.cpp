#include "Hoverable.h"
#include "../../application/ApplicationManager.h"

void Hix::Input::Hoverable::onMouseMovement(const Qt3DRender::QRayCasterHit&)
{
}

bool Hix::Input::Hoverable::mouseMovementEnabled()
{
	return _isMovementEnabled;
}

//because we need to remove this hoverable pointer from raycaster when deleting
Hix::Input::Hoverable::~Hoverable()
{
	Hix::Application::ApplicationManager::getInstance().getRayCaster().hoverObjectDeleted(this);
}
