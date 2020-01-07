#include "Inputs.h"

using namespace Hix::QML;

Hix::QML::InputBox::InputBox(QQuickItem* parent) : QQuickItem(parent)
{
	_label = new QQuickText(this);
}

Hix::QML::DropdownBox::DropdownBox(QQuickItem* parent) : _dropRect(new QQuickComboBox(this))
{
	setParent(parent);
}

void Hix::QML::DropdownBox::dropNameChanged()
{
	qDebug() << _dropName;
}
