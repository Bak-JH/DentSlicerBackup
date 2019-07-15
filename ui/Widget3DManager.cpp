#include "Widget3DManager.h"
#include "../qmlmanager.h"
#include "../input/raycastcontroller.h"
#include <Qt3DCore>
using namespace Hix;
using namespace Hix::UI;

Widget3DManager::Widget3DManager(Qt3DCore::QEntity* qParent, Input::RayCastController* controller)
{
	_rotateWidget.setParent(qParent);
	controller->addLayer(&_rotateWidget.layer);
	controller->addHoverLayer(&_rotateWidget.layer);
	hideRotateSphere();

	//move widget
	_moveWidget.setParent(qParent);
	controller->addLayer(&_moveWidget.layer);
	controller->addHoverLayer(&_moveWidget.layer);
	hideMoveArrow();

}


