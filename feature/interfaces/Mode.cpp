#include "Mode.h"
#include "../../qml/components/Buttons.h"
#include "../../application/ApplicationManager.h"
using namespace Hix::Features;

Hix::Features::Mode::Mode()
{
}


void Hix::Features::Mode::addButton(Hix::QML::Controls::ToggleSwitch* button)
{
	_button = button;
}

void Hix::Features::Mode::scheduleForDelete()
{
	QMetaObject::invokeMethod(&Hix::Application::ApplicationManager::getInstance().engine(), []() {
		Hix::Application::ApplicationManager::getInstance().featureManager().setMode(nullptr);
	}, Qt::QueuedConnection);

}

Hix::Features::Mode::~Mode()
{
	if (_button)
	{
		_button->setChecked(false);
	}
}

Hix::Features::InstantMode::InstantMode()
{
}

Hix::Features::InstantMode::~InstantMode()
{
}
