#include "Inputs.h"
#include <codecvt>
#include <locale>

using namespace Hix::QML::Controls;

Hix::QML::Controls::InputSpinBox::InputSpinBox(QQuickItem* parent) : QQuickItem(parent)
{
}

Hix::QML::Controls::InputSpinBox::~InputSpinBox()
{
}

double Hix::QML::Controls::InputSpinBox::getValue() const
{
	return _value;
}

void Hix::QML::Controls::InputSpinBox::setValue(double value)
{
	_value = value;
	emit valueChanged();
}

void Hix::QML::Controls::InputSpinBox::setRange(double min, double max)
{
	_min = min;
	_max = max;
	emit rangeChanged();
}

Hix::QML::Controls::TextInputBox::TextInputBox(QQuickItem* parent) : QQuickItem(parent)
{
}

Hix::QML::Controls::TextInputBox::~TextInputBox()
{
}

std::string Hix::QML::Controls::TextInputBox::getInputText() const
{
	std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> utf16_to_utf8;
	return utf16_to_utf8.to_bytes(_text.toStdU16String());
}	

void Hix::QML::Controls::TextInputBox::setInputText(std::string text)
{
	_text.fromStdString(text);
	emit inputTextChanged();
}

Hix::QML::Controls::DropdownBox::DropdownBox(QQuickItem* parent) : QQuickItem(parent)
{
}

Hix::QML::Controls::DropdownBox::~DropdownBox()
{
}

int Hix::QML::Controls::DropdownBox::getIndex() const
{
	return _index;
}

void Hix::QML::Controls::DropdownBox::setIndex(int index)
{
	_index = index;
	emit indexChanged();
}

void Hix::QML::Controls::DropdownBox::setList(QStringList list)
{
	_dropList = list;
	emit listChanged();
}
