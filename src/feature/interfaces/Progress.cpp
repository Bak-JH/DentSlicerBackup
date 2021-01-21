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

QString Hix::Progress::getDisplayText()
{
	return QString::fromStdString(_displayText);
}
