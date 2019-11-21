#include "Widget3DManager.h"
#include "qmlmanager.h"
#include "RotateWidget.h"
#include "MoveWidget.h"
#include "CubeWidget.h"

using namespace Hix;
using namespace Hix::UI;

void Widget3DManager::initialize(Qt3DCore::QEntity* qParent, Input::RayCastController* controller)
{
	//add each individual widgets
	_rotateWidget.addWidget(std::make_unique<RotateWidget>(QVector3D(1, 0, 0), &_rotateWidget));
	_rotateWidget.addWidget(std::make_unique<RotateWidget>(QVector3D(0, 1, 0), &_rotateWidget));
	_rotateWidget.addWidget(std::make_unique<RotateWidget>(QVector3D(0, 0, 1), &_rotateWidget));

	_moveWidget.addWidget(std::make_unique<MoveWidget>(QVector3D(1, 0, 0), &_moveWidget));
	_moveWidget.addWidget(std::make_unique<MoveWidget>(QVector3D(0, 1, 0), &_moveWidget));

	_controller = controller;

	_rotateWidget.setParent(qParent);
	_rotateWidget.setVisible(false);

	_moveWidget.setParent(qParent);
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


