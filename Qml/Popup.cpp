#include "Popup.h"

Hix::QML::XButton::XButton(QQuickItem* parent) : _mouseArea(new QQuickMouseArea(this))
{
	setParent(parent);
	connect(_mouseArea, &QQuickMouseArea::clicked, this, &XButton::onClick);
	qvariant_cast<QObject*>(
		_mouseArea->property("anchors")
		)->setProperty("fill", _mouseArea->property("parent"));
}

void Hix::QML::XButton::onClick()
{
	qDebug() << "clicked";
}

void Hix::QML::PopupShell::titleChanged()
{
	qDebug() << _title;
}

Hix::QML::PopupShell::PopupShell(QQuickItem* parent)
{
	setParent(parent);
}

