#include "Buttons.h"

using namespace Hix::QML;

/// Button baseclass ///
Hix::QML::Button::Button(QQuickItem* parent) : QQuickRectangle(parent), _mouseArea(new QQuickMouseArea(this))
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

Hix::QML::Button::~Button()
{
}

void Hix::QML::Button::onClicked()
{
	emit clicked();
}

void Hix::QML::Button::onEntered()
{
	emit entered();
}

void Hix::QML::Button::onExited()
{
	emit exited();
}



/// Close Button ///
Hix::QML::CloseButton::CloseButton(QQuickItem* parent) : Button(parent)
{
}

Hix::QML::CloseButton::~CloseButton()
{
}

void Hix::QML::CloseButton::onClicked()
{
	parentItem()->parentItem()->setVisible(false);
	Button::onClicked();
}

/// Toggle Switch ///
Hix::QML::ToggleSwitch::ToggleSwitch(QQuickItem* parent) :Button(parent)
{
}

Hix::QML::ToggleSwitch::~ToggleSwitch()
{
}

void Hix::QML::ToggleSwitch::initialize(QVariant leftVal, QVariant rightVal)
{
}

QVariant Hix::QML::ToggleSwitch::value() const
{
	if (_isLeft)
		return _leftVal;
	else
		return _rightVal;
}


void Hix::QML::ToggleSwitch::onClicked()
{
	Button::onClicked();
}

