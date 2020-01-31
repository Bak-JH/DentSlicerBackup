#include "Widget3D.h"
#include "../qmlmanager.h"
#include "../input/raycastcontroller.h"
#include <math.h>
#include "Widget.h"

using namespace Hix::UI;
using namespace Hix::Input;
Widget3D::Widget3D(Hix::Features::WidgetMode* mode) : _mode(mode)
{
	addComponent(&_transform);
}

Hix::UI::Widget3D::~Widget3D()
{
}

void Hix::UI::Widget3D::addWidget(std::unique_ptr<Widget> widget)
{

	_widgets.push_back(std::move(widget));
}

void Hix::UI::Widget3D::setVisible(bool show)
{
	if (_visible != show)
	{
		_visible = show;
		if (_visible)
		{
			setEnabled(true);
			for (auto& each : _widgets)
			{
				each->setHitTestable(true);
				each->setHoverable(true);
			}
		}
		else
		{
			setEnabled(false);
			for (auto& each : _widgets)
			{
				each->setHitTestable(false);
				each->setHoverable(false);
			}
		}
	}
}

Qt3DCore::QTransform* Hix::UI::Widget3D::transform()
{
	return &_transform;
}
bool Hix::UI::Widget3D::isManipulated()
{
	return _isManipulated;
}
void Hix::UI::Widget3D::setManipulated(bool isManipulated)
{
	if (_isManipulated != isManipulated)
	{
		_isManipulated = isManipulated;
		for (auto& each : _widgets)
		{
			each->setHighlight(false);
		}
	}
}

Hix::Features::WidgetMode* Hix::UI::Widget3D::mode()
{
	return _mode;
}

bool Hix::UI::Widget3D::visible()
{
	return _visible;
}






