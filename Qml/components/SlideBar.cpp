#include "SlideBar.h"


Hix::QML::SlideBarShell::SlideBarShell(QQuickItem* parent) : QQuickItem(parent)
{
}

Hix::QML::SlideBarShell::~SlideBarShell()
{
}

void Hix::QML::SlideBarShell::setMin(double pMin)
{
	_min = pMin;
	emit minimumChanged();
}

void Hix::QML::SlideBarShell::setMax(double pMax)
{
	_max = pMax;
	emit maximumChanged();
}

double Hix::QML::SlideBarShell::getMin() const
{
	return _min;
}

double Hix::QML::SlideBarShell::getMax() const
{
	return _max;
}

double Hix::QML::SlideBarShell::getValue() const
{
	return _value;
}



Hix::QML::RangeSlideBarShell::RangeSlideBarShell(QQuickItem* parent) : QQuickItem(parent)
{
}

Hix::QML::RangeSlideBarShell::~RangeSlideBarShell()
{
}

void Hix::QML::RangeSlideBarShell::setMin(double pMin)
{
	_min = pMin;
	emit minimumChanged();
}

void Hix::QML::RangeSlideBarShell::setMax(double pMax)
{
	_max = pMax;
	emit maximumChanged();
}

double Hix::QML::RangeSlideBarShell::getMin() const
{
	return _min;
}

double Hix::QML::RangeSlideBarShell::getMax() const
{
	return _max;
}

double Hix::QML::RangeSlideBarShell::lowerValue() const
{
	return _lowerValue;
}

double Hix::QML::RangeSlideBarShell::upperValue() const
{
	return _upperValue;
}
