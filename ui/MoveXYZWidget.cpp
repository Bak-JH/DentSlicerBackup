#include "MoveXYZWidget.h"
#include "../qmlmanager.h"
#include "../input/raycastcontroller.h"
#include <math.h>

using namespace Hix::UI;
MoveXYZWidget::MoveXYZWidget():_widgets{ {{QVector3D(1,0,0), this},  {QVector3D(0,1,0), this}} }
{
	addComponent(&_transform);
	layer.setRecursive(false);
	for (auto& each : _widgets)
	{
		each.addComponent(&layer);
	}
}

Hix::UI::MoveXYZWidget::~MoveXYZWidget()
{

}

void Hix::UI::MoveXYZWidget::setVisible(bool show)
{
	if (_visible != show)
	{
		_visible = show;
		if (_visible)
		{
			_center = qmlManager->getSelectedCenter();
			updatePosition();
			setEnabled(true);
		}
		else
		{
			setEnabled(false);
		}
	}
}

Qt3DCore::QTransform* Hix::UI::MoveXYZWidget::transform()
{
	return &_transform;
}
bool Hix::UI::MoveXYZWidget::isManipulated()
{
	return _isManipulated;
}
void Hix::UI::MoveXYZWidget::setManipulated(bool isManipulated)
{
	_isManipulated = isManipulated;
}
bool Hix::UI::MoveXYZWidget::visible()
{
	return _visible;
}

void Hix::UI::MoveXYZWidget::updatePosition()
{
	if (_visible)
	{
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




