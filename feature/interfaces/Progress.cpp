#include "Progress.h"

Hix::Progress::Progress()
{
}

Hix::Progress::~Progress()
{
}

void Hix::Progress::setDisplayText(std::string text)
{
	_displayText = text;
}

std::string Hix::Progress::getDisplayText()
{
	return _displayText;
}
