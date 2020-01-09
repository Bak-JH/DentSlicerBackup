#include "Buttons.h"

using namespace Hix::QML;

/// Button baseclass ///
Hix::QML::Button::Button(QQuickItem* parent) : QQuickRectangle(parent)
{
	// mouse area
	connect(_mouseArea, &QQuickMouseArea::clicked, this, &Button::onClick);
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

void Hix::QML::Button::onEntered()
{
}

void Hix::QML::Button::onExited()
{
}



/// Close Button ///
Hix::QML::CloseButton::CloseButton(QQuickItem* parent) : Button(parent)
{
	_image = new QQuickImage(this);
}

Hix::QML::CloseButton::~CloseButton()
{
}

void Hix::QML::CloseButton::onClick()
{
	qDebug() << parentItem()->parentItem()->objectName();
	parentItem()->parentItem()->setVisible(false);
}


/// Round Button ///
Hix::QML::RoundButton::RoundButton(QQuickItem* parent) :Button(parent)
{
	_labelText = new QQuickText(this);
}

Hix::QML::RoundButton::~RoundButton()
{
}

void Hix::QML::RoundButton::onClick()
{
	emit clicked();
}
void Hix::QML::RoundButton::onEntered()
{
	emit entered();
}
void Hix::QML::RoundButton::onExited()
{
	emit exited();
}
void Hix::QML::RoundButton::fNameChanged()
{
	emit fNameChanged();
}



/// Menu Button ///
Hix::QML::MenuButton::MenuButton(QQuickItem* parent) :Button(parent)
{
	_name = new QQuickText(this);
	_image = new QQuickImage(this);
}

Hix::QML::MenuButton::~MenuButton()
{
}

void Hix::QML::RoundButton::onClick()
{
}

void Hix::QML::MenuButton::onEntered()
{
}

void Hix::QML::MenuButton::onExited()
{
}
