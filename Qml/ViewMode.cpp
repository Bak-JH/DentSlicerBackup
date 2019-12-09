#include "ViewMode.h"

Hix::QML::ViewModeShell::ViewModeShell(QQuickItem* parent)
{
	setParent(parent);
}

Hix::QML::ViewModePopup::ViewModePopup(QQuickItem* parent)
{
	setParent(parent);
}

void Hix::QML::ViewModeShell::viewtypeChanged()
{
	qDebug() << _viewtype;
}

void Hix::QML::ObjectViewButton::onClick()
{
	qDebug() << "clicked";
}

void Hix::QML::LayerViewButton::onClick()
{
	qDebug() << "clicked";
}

Hix::QML::ObjectViewButton::ObjectViewButton(QQuickItem* parent) : _mouseArea(new QQuickMouseArea(this))
{
	setParent(parent);
	connect(_mouseArea, &QQuickMouseArea::clicked, this, &ObjectViewButton::onClick);
	qvariant_cast<QObject*>(
		_mouseArea->property("anchors")
		)->setProperty("fill", _mouseArea->property("parent"));
}

Hix::QML::LayerViewButton::LayerViewButton(QQuickItem* parent) : _mouseArea(new QQuickMouseArea(this))
{
	setParent(parent);
	connect(_mouseArea, &QQuickMouseArea::clicked, this, &LayerViewButton::onClick);
	qvariant_cast<QObject*>(
		_mouseArea->property("anchors")
		)->setProperty("fill", _mouseArea->property("parent"));
}