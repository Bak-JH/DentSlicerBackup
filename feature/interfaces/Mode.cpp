#include "Mode.h"
#include "../../qml/components/Buttons.h"
using namespace Hix::Features;

Hix::Features::Mode::Mode()
{

}

void Hix::Features::Mode::addButton(Hix::QML::Controls::Button* button)
{
	_button = button;
}
	
Hix::Features::Mode::~Mode()
{
	if (_button)
	{
		emit _button->clicked();
	}
}

Hix::Features::InstantMode::InstantMode()
{
}

Hix::Features::InstantMode::~InstantMode()
{
}
