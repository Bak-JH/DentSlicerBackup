#include "MenuButton.h"

Hix::QML::MenuBtnShell::MenuBtnShell(QQuickItem* parent) : _mouseArea(new QQuickMouseArea(this))
{
	setParent(parent);
	connect(_mouseArea, &QQuickMouseArea::clicked, this, &MenuBtnShell::onClick);
	qvariant_cast<QObject*>(
		_mouseArea->property("anchors")
		)->setProperty("fill", _mouseArea->property("parent"));

}

void Hix::QML::MenuBtnShell::onClick()
{
	qDebug() << "clicked";
}
