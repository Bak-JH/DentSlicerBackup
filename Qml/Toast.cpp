#include "Toast.h"

void Hix::QML::ToastShell::toastmsgChanged()
{
	qDebug() << _toastmsg;
}

Hix::QML::ToastShell::ToastShell(QQuickItem* parent)
{
	setParent(parent);
}

void Hix::QML::ToastCloseButton::onClick()
{
	qDebug() << "close clicked";
}

Hix::QML::ToastCloseButton::ToastCloseButton(QQuickItem* parent) : _mouseArea(new QQuickMouseArea(this))
{
	setParent(parent);
	connect(_mouseArea, &QQuickMouseArea::clicked, this, &ToastCloseButton::onClick);
	qvariant_cast<QObject*>(
		_mouseArea->property("anchors")
		)->setProperty("fill", _mouseArea->property("parent"));
}