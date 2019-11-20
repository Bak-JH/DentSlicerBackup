#include "LeftPopup.h"


Hix::QML::CloseButton::CloseButton(QQuickItem* parent) : _mouseArea(new QQuickMouseArea(this))
{
	setParent(parent);
	connect(_mouseArea, &QQuickMouseArea::clicked, this, &CloseButton::onClick);
	qvariant_cast<QObject*>(
		_mouseArea->property("anchors")
		)->setProperty("fill", _mouseArea->property("parent"));
}

Hix::QML::RoundButton::RoundButton(QQuickItem* parent) : _mouseArea(new QQuickMouseArea(this))
{
	connect(_mouseArea, &QQuickMouseArea::clicked, this, &RoundButton::onClick);
	qvariant_cast<QObject*>(
		_mouseArea->property("anchors")
		)->setProperty("fill", _mouseArea->property("parent"));
}


void Hix::QML::CloseButton::onClick()
{
	qDebug() << "close clicked";
}

void Hix::QML::RoundButton::onClick()
{
	qDebug() << "round button clicked";
}

void Hix::QML::LeftPopupShell::titleChanged()
{
	qDebug() << _title;
}

void Hix::QML::LeftPopupShell::popupHeightChanged()
{
	qDebug() << _popupHeight;
}

void Hix::QML::InputBox::propNameChanged()
{
	qDebug() << _propName;
}

void Hix::QML::RoundButton::textChanged()
{
	qDebug() << _btntext;
}

Hix::QML::LeftPopupShell::LeftPopupShell(QQuickItem* parent)
{
	setParent(parent);
}

Hix::QML::InputBox::InputBox(QQuickItem* parent):_inputRect(new QQuickSpinBox(this))
{
	setParent(parent);
}

