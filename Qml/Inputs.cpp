#include "Inputs.h"

using namespace Hix::QML;

Hix::QML::InputBox::InputBox(QQuickItem* parent) : QQuickItem(parent)
{
	_label = new QQuickText(this);
	_spinbox = new InputSpinBox(this);
}

Hix::QML::DropdownBox::DropdownBox(QQuickItem* parent) : _dropRect(new QQuickComboBox(this))
{
	setParent(parent);
}

Hix::QML::InputSpinBox::InputSpinBox(QQuickItem* parent) : QQuickSpinBox(parent)
{

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

Hix::QML::InputSpinBox* Hix::QML::InputBox::spinbox() const
{
	return _spinbox;
}
