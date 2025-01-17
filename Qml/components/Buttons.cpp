#include "Buttons.h"

using namespace Hix::QML::Controls;

/// Button baseclass ///
Hix::QML::Controls::Button::Button(QQuickItem* parent) : QQuickItem(parent)
{
	// mouse area
	//connect(_mouseArea, &QQuickMouseArea::clicked, this, &Button::onClicked);
	//connect(_mouseArea, &QQuickMouseArea::entered, this, &Button::onEntered);
	//connect(_mouseArea, &QQuickMouseArea::exited, this, &Button::onExited);
	//connect(_mouseArea, &QQuickMouseArea::positionChanged, this, &Button::onPositionChanged);

	//qvariant_cast<QObject*>(
	//	_mouseArea->property("anchors")
	//	)->setProperty("fill", _mouseArea->property("parent"));
	//_mouseArea->setHoverEnabled(true);
}

Hix::QML::Controls::Button::~Button()
{
}

void Hix::QML::Controls::Button::onClicked()
{
	emit clicked();
}

//void Hix::QML::Controls::Button::onEntered()
//{
//	emit entered();
//}
//
//void Hix::QML::Controls::Button::onExited()
//{
//	emit exited();
//}

//void Hix::QML::Controls::Button::onPositionChanged(QQuickMouseEvent* mouse)
//{
//	emit positionChanged(mouse);
//}
//



/// Toggle Switch ///
Hix::QML::Controls::ToggleSwitch::ToggleSwitch(QQuickItem* parent) : QQuickItem(parent)
{
	// mouse area

	connect(this, &ToggleSwitch::checkedChanged, this, &ToggleSwitch::onCheckedChanged);
}

Hix::QML::Controls::ToggleSwitch::~ToggleSwitch()
{
}



bool Hix::QML::Controls::ToggleSwitch::isChecked() const
{
	return _isChecked;
}

void Hix::QML::Controls::ToggleSwitch::setChecked(bool isChecked)
{
	if (isChecked != _isChecked)
	{
		_isChecked = isChecked;
		//if (_isChecked)
		//	emit checked();
		//else
		//	emit unchecked();
		emit checkedChanged();
	}
}

void Hix::QML::Controls::ToggleSwitch::onCheckedChanged()
{
	if (_isChecked)
		emit checked();
	else
		emit unchecked();
}

void Hix::QML::Controls::ToggleSwitch::onEntered()
{
	emit entered();
}

void Hix::QML::Controls::ToggleSwitch::onExited()
{
	emit exited();
}
//void Hix::QML::Controls::ToggleSwitch::onClicked()
//{
//	setChecked(!_isChecked);
//}
