#include "Widget3D.h"
#include "../qmlmanager.h"
#include "../input/raycastcontroller.h"
#include <math.h>
#include "Highlightable.h"

using namespace Hix::UI;
using namespace Hix::Input;
Widget3D::Widget3D()
{
	addComponent(&_transform);
	layer.setRecursive(false);
}

Hix::UI::Widget3D::~Widget3D()
{
}

void Hix::UI::Widget3D::addWidget(std::unique_ptr<Highlightable> widget)
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
			updatePosition();
			setEnabled(true);
		}
		else
		{
			setEnabled(false);
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
bool Hix::UI::Widget3D::visible()
{
	return _visible;
}

void Hix::UI::Widget3D::updatePosition()
{
	if (_visible)
	{
		_center = qmlManager->getSelectedCenter();
		auto syszoom = qmlManager->systemTransform->scale3D();

		_transform.setScale3D(QVector3D(0.01 / syszoom.x(), 0.01 / syszoom.y(), 0.01 / syszoom.z()));

		auto theta = qmlManager->systemTransform->rotationX() / 180.0 * M_PI;
		auto alpha = qmlManager->systemTransform->rotationZ() / 180.0 * M_PI;
		_transform.setTranslation(QVector3D(
			_center.x() + 100 * std::sin(theta) * std::sin(alpha),
			_center.y() + 100 * std::sin(theta) * std::cos(alpha),
			_center.z() + 100 * std::cos(theta)));
	}
}




