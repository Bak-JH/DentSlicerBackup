#include "PartList.h"

Hix::QML::PartListContent::PartListContent(QQuickItem* parent) : _mouseArea(new QQuickMouseArea(this))
{
	setParent(parent);
	connect(_mouseArea, &QQuickMouseArea::clicked, this, &PartListContent::onClick);
	qvariant_cast<QObject*>(
		_mouseArea->property("anchors")
		)->setProperty("fill", _mouseArea->property("parent"));

}

void Hix::QML::PartListContent::onClick()
{
	qDebug() << "part clicked";
}

void Hix::QML::PartDeleteButton::onClick()
{
	qDebug() << "part clicked";
}

Hix::QML::PartDeleteButton::PartDeleteButton(QQuickItem* parent) : _mouseArea(new QQuickMouseArea(this))
{
	setParent(parent);
	connect(_mouseArea, &QQuickMouseArea::clicked, this, &PartDeleteButton::onClick);
	qvariant_cast<QObject*>(
		_mouseArea->property("anchors")
		)->setProperty("fill", _mouseArea->property("parent"));

}