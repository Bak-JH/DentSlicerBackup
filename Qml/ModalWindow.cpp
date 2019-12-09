#include "ModalWindow.h"

void Hix::QML::ModalShell::modalmsgChanged()
{
	qDebug() << _modalmsg;
}

Hix::QML::ModalShell::ModalShell(QQuickItem* parent)
{
	setParent(parent);
}

void Hix::QML::ModalCloseButton::onClick()
{
	qDebug() << "close clicked";
}

void Hix::QML::ModalRoundButton::onClick()
{
	qDebug() << "close clicked";
}

void Hix::QML::ModalRoundButton::modalbtntextChanged()
{
	qDebug() << _modalbtntext;
}

Hix::QML::ModalRoundButton::ModalRoundButton(QQuickItem* parent)
{
	setParent(parent);
}

Hix::QML::ModalCloseButton::ModalCloseButton(QQuickItem* parent) : _mouseArea(new QQuickMouseArea(this))
{
	setParent(parent);
	connect(_mouseArea, &QQuickMouseArea::clicked, this, &ModalCloseButton::onClick);
	qvariant_cast<QObject*>(
		_mouseArea->property("anchors")
		)->setProperty("fill", _mouseArea->property("parent"));
}