#include "WidgetMode.h"
#include "qmlmanager.h"
#include "glmodel.h"

Hix::Features::WidgetMode::WidgetMode()
	: _controller(&qmlManager->getRayCaster()), _widget(this)
{
	_widget.setParent(qmlManager->total);
	_controller->setHoverEnabled(true);
	_widget.setVisible(true);
}

Hix::Features::WidgetMode::~WidgetMode()
{
	_controller->setHoverEnabled(false);
}

void Hix::Features::WidgetMode::updatePosition()
{
	if (_widget.visible())
	{
		auto syszoom = qmlManager->systemTransform->scale3D();
		auto transform = _widget.transform();
		auto centre = getWidgetPosition();
		transform->setScale3D(QVector3D(0.01 / syszoom.x(), 0.01 / syszoom.y(), 0.01 / syszoom.z()));
		auto theta = qmlManager->systemTransform->rotationX() / 180.0 * M_PI;
		auto alpha = qmlManager->systemTransform->rotationZ() / 180.0 * M_PI;
		transform->setTranslation(QVector3D(
			centre.x() + 100 * std::sin(theta) * std::sin(alpha),
			centre.y() + 100 * std::sin(theta) * std::cos(alpha),
			centre.z() + 100 * std::cos(theta)));
	}
}

