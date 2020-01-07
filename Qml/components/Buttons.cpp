#include "Buttons.h"

using namespace Hix::QML;

/// Close Button ///
Hix::QML::CloseButton::CloseButton(QQuickItem* parent) : QQuickRectangle(parent), _mouseArea(new QQuickMouseArea(this))
{
	_image = new QQuickImage(this);

	// mouse area
	connect(_mouseArea, &QQuickMouseArea::clicked, this, &CloseButton::onClick);
	qvariant_cast<QObject*>(
		_mouseArea->property("anchors")
		)->setProperty("fill", _mouseArea->property("parent"));
}

void Hix::QML::CloseButton::onClick()
{
	qDebug() << parentItem()->parentItem()->objectName();
	parentItem()->parentItem()->setVisible(false);
}


/// Round Button ///
Hix::QML::RoundButton::RoundButton(QQuickItem* parent) :QQuickRectangle(parent), _mouseArea(new QQuickMouseArea(this))
{
	_labelText = new QQuickText(this);

	// mouse area
	connect(_mouseArea, &QQuickMouseArea::clicked, this, &RoundButton::onClick);
	connect(_mouseArea, &QQuickMouseArea::entered, this, &RoundButton::onEntered);
	connect(_mouseArea, &QQuickMouseArea::exited, this, &RoundButton::onExited);

	_mouseArea->setHoverEnabled(true);
	qvariant_cast<QObject*>(
		_mouseArea->property("anchors")
		)->setProperty("fill", _mouseArea->property("parent"));
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