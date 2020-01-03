#include "Inputs.h"

using namespace Hix::QML;

Hix::QML::InputBox::InputBox(QQuickItem* parent) : QQuickSpinBox(parent)
{
	_inputLabel = new QQuickText(this);
}

Hix::QML::DropdownBox::DropdownBox(QQuickItem* parent) : _dropRect(new QQuickComboBox(this))
{
	setParent(parent);
}

/*
void Hix::QML::InputBox::propNameChanged()
{
	qDebug() << _propName;
}
*/
void Hix::QML::DropdownBox::dropNameChanged()
{
	qDebug() << _dropName;
}