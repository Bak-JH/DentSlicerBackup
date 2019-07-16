#include "Widget3DManager.h"
#include "../qmlmanager.h"
#include "../input/raycastcontroller.h"
#include <Qt3DCore>
using namespace Hix;
using namespace Hix::UI;

void Widget3DManager::initialize(Qt3DCore::QEntity* qParent, Input::RayCastController* controller)
{
	_controller = controller;
	_rotateWidget.setParent(qParent);
	controller->addLayer(&_rotateWidget.layer);
	controller->addHoverLayer(&_rotateWidget.layer);
	_rotateWidget.setVisible(false);

	//move widget
	_moveWidget.setParent(qParent);
	controller->addLayer(&_moveWidget.layer);
	controller->addHoverLayer(&_moveWidget.layer);
	_moveWidget.setVisible(false);

}


void Hix::UI::Widget3DManager::setWidgetMode(WidgetMode mode)
{
	if (_currMode != mode)
	{
		switch (mode)
		{
			case WidgetMode::Move:
			{
				qDebug() << "WidgetMode::Move";
				_moveWidget.setVisible(true);
				_rotateWidget.setVisible(false);
				_controller->setHoverEnabled(true);
				break;
			}
			case WidgetMode::Rotate:
			{
				qDebug() << "WidgetMode::Rotate";
				_moveWidget.setVisible(false);
				_rotateWidget.setVisible(true);
				_controller->setHoverEnabled(true);
				break;
			}
			case WidgetMode::None:
			{
				qDebug() << "WidgetMode::None";
				_moveWidget.setVisible(false);
				_rotateWidget.setVisible(false);
				_controller->setHoverEnabled(false);
				break;
			}
		}
		_currMode = mode;
	}
}

void Hix::UI::Widget3DManager::updatePosition()
{
	switch (_currMode)
	{
	case WidgetMode::Move:
	{
		_moveWidget.updatePosition();
		break;
	}
	case WidgetMode::Rotate:
	{
		_rotateWidget.updatePosition();
		break;
	}
	case WidgetMode::None:
	{
		break;
	}
	}
}


