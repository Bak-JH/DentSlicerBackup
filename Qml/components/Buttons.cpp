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

bool Hix::QML::ToggleSwitch::isChecked() const
{
	return _isChecked;
}

void Hix::QML::ToggleSwitch::onClicked()
{
	_isChecked = _isChecked ? false : true;
	Button::onClicked();
}

/// Image Toggle Switch ///
Hix::QML::ImageToggleSwitch::ImageToggleSwitch(QQuickItem* parent) : QQuickItem(parent)
{
}

Hix::QML::ImageToggleSwitch::~ImageToggleSwitch()
{
}

bool Hix::QML::ImageToggleSwitch::isChecked() const
{
	return _isChecked;
}

Q_INVOKABLE void Hix::QML::ImageToggleSwitch::setChecked(bool isChecked)
{
	_isChecked = isChecked;
}
