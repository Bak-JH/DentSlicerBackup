#include "Cam.h"


Hix::QML::CamPopupShell::CamPopupShell(QQuickItem* parent)
{
	setParent(parent);
}


void Hix::QML::CamButtonShell::onClick()
{
	qDebug() << "cam clicked";
}

void Hix::QML::CamBoxIcon::onClick()
{
	qDebug() << "cam icon clicked";
}


Hix::QML::CamButtonShell::CamButtonShell(QQuickItem* parent) : _mouseArea(new QQuickMouseArea(this))
{
	setParent(parent);
	connect(_mouseArea, &QQuickMouseArea::clicked, this, &CamButtonShell::onClick);
	qvariant_cast<QObject*>(
		_mouseArea->property("anchors")
		)->setProperty("fill", _mouseArea->property("parent"));
}

Hix::QML::CamBoxIcon::CamBoxIcon(QQuickItem* parent) : _mouseArea(new QQuickMouseArea(this))
{
	setParent(parent);
	connect(_mouseArea, &QQuickMouseArea::clicked, this, &CamBoxIcon::onClick);
	qvariant_cast<QObject*>(
		_mouseArea->property("anchors")
		)->setProperty("fill", _mouseArea->property("parent"));
}