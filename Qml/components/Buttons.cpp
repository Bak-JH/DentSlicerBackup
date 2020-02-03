#include "Buttons.h"

using namespace Hix::QML::Controls;

/// Button baseclass ///
Hix::QML::Controls::Button::Button(QQuickItem* parent) : QQuickRectangle(parent), _mouseArea(new QQuickMouseArea(this))
{
	// mouse area
	connect(_mouseArea, &QQuickMouseArea::clicked, this, &Button::onClicked);
	connect(_mouseArea, &QQuickMouseArea::entered, this, &Button::onEntered);
	connect(_mouseArea, &QQuickMouseArea::exited, this, &Button::onExited);

	qvariant_cast<QObject*>(
		_mouseArea->property("anchors")
		)->setProperty("fill", _mouseArea->property("parent"));
	_mouseArea->setHoverEnabled(true);
}

Hix::QML::Controls::Button::~Button()
{
}

void Hix::QML::Controls::Button::onClicked()
{
	emit clicked();
}

void Hix::QML::Controls::Button::onEntered()
{
	emit entered();
}

void Hix::QML::Controls::Button::onExited()
{
	emit exited();
}




/// Toggle Switch ///
Hix::QML::Controls::ToggleSwitch::ToggleSwitch(QQuickItem* parent) :Button(parent)
{
}

Hix::QML::Controls::ToggleSwitch::~ToggleSwitch()
{
}

void Hix::QML::Controls::ToggleSwitch::initialize(QVariant leftVal, QVariant rightVal)
{
}

void Hix::QML::Controls::ToggleSwitch::onClicked()
{
	_isChecked = !_isChecked;
	Button::onClicked();
}

