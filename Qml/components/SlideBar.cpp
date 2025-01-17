#include "SlideBar.h"


Hix::QML::SlideBarShell::SlideBarShell(QQuickItem* parent) : QQuickItem(parent)
{
}

Hix::QML::SlideBarShell::~SlideBarShell()
{
}

void Hix::QML::SlideBarShell::setRange(double min, double max)
{
	if (_min != min)
	{
		_min = min;
		emit minimumChanged();
	}

	if (_max != max)
	{
		_max = max;
		emit maximumChanged();
	}
}

void Hix::QML::SlideBarShell::setValue(double value)
{
	_value = value;
	emit valueChanged();
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

void Hix::QML::RangeSlideBarShell::setLowerValue(double value)
{
	_lowerValue = value;
	emit lowerValueChanged();
}

void Hix::QML::RangeSlideBarShell::setUpperValue(double value)
{
	_upperValue = value;
	emit upperValueChanged();
}
