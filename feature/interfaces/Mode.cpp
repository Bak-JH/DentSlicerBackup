#include "Mode.h"
#include "../../qml/components/Buttons.h"
using namespace Hix::Features;

Hix::Features::Mode::Mode()
{
}


void Hix::Features::Mode::addButton(Hix::QML::Controls::ToggleSwitch* button)
{
	_button = button;
	if (_button)
	{
		_button->setChecked(true);
	}
}

Hix::Features::Mode::~Mode()
{
	if (_button)
	{
		_button->setChecked(false);
		emit _button->unchecked();
	}
}

Hix::Features::InstantMode::InstantMode()
{
}

Hix::Features::InstantMode::~InstantMode()
{
}
