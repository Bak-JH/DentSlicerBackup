#include "WidgetMode.h"
#include "application/ApplicationManager.h"
#include "glmodel.h"

Hix::Features::WidgetMode::WidgetMode()
	: _controller(&Hix::Application::ApplicationManager::getInstance().getRayCaster()), _widget(this)
{
	_widget.setParent(Hix::Application::ApplicationManager::getInstance().sceneManager().total());
	_controller->setHoverEnabled(true);
}

Hix::Features::WidgetMode::~WidgetMode()
{
	_controller->setHoverEnabled(false);
}

void Hix::Features::WidgetMode::updatePosition()
{
	if (_widget.visible())
	{
		auto transform = _widget.transform();
		updateTransform(*transform);
	}
}

void Hix::Features::WidgetMode::updateTransform(Qt3DCore::QTransform& transform)
{
	auto sysTransform = Hix::Application::ApplicationManager::getInstance().sceneManager().systemTransform();
	auto syszoom = sysTransform->scale3D();
	auto centre = getWidgetPosition();
	transform.setScale3D(QVector3D(0.01 / syszoom.x(), 0.01 / syszoom.y(), 0.01 / syszoom.z()));
	auto theta = sysTransform->rotationX() / 180.0 * M_PI;
	auto alpha = sysTransform->rotationZ() / 180.0 * M_PI;
	qDebug() << "camera angle x:" << sysTransform->rotationX() << "  y:" << sysTransform->rotationY() << "  z:" << sysTransform->rotationZ();
	transform.setTranslation(QVector3D(
		centre.x() + 100 * std::sin(theta) * std::sin(alpha),
		centre.y() + 100 * std::sin(theta) * std::cos(alpha),
		centre.z() + 100 * std::cos(theta)));
}

void Hix::Features::UpdateWidgetModePos()
{
	auto widgetMode = dynamic_cast<WidgetMode*>(Hix::Application::ApplicationManager::getInstance().featureManager().currentMode());
	if (widgetMode)
	{
		widgetMode->updatePosition();
	}
}
