#include "Widget.h"
#include "Widget3D.h"
Hix::UI::Widget::Widget(const QVector3D& axis, Qt3DCore::QEntity* parent): QEntity(parent), _axis(axis),
_parent(dynamic_cast<Widget3D*>(parent))
{
	initHitTest();
	setEnabled(true);
}
void Hix::UI::Widget::initHitTest()
{
	addComponent(&_layer);
	_layer.setRecursive(false);
}

Hix::UI::Widget::~Widget()
{
}
