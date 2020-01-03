#include "WidgetMode.h"
#include "qmlmanager.h"
#include "glmodel.h"

Hix::Features::WidgetMode::WidgetMode(const std::unordered_set<GLModel*>& targetModels, Input::RayCastController* controller)
	: _targetModels(targetModels), _controller(controller), _widget(this)
{
	_widget.setParent(qmlManager->total);
	_controller->setHoverEnabled(true);
}

Hix::Features::WidgetMode::~WidgetMode()
{
	_controller->setHoverEnabled(false);
}

void Hix::Features::WidgetMode::updatePosition()
{
	_widget.updatePosition();
}

const std::unordered_set<GLModel*>& Hix::Features::WidgetMode::models() const
{
	return _targetModels;
}
 