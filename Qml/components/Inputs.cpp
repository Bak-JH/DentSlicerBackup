#include "Inputs.h"
#include <codecvt>
#include <locale>

using namespace Hix::QML;

Hix::QML::InputSpinBox::InputSpinBox(QQuickItem* parent) : QQuickItem(parent)
{
}

Hix::QML::InputSpinBox::~InputSpinBox()
{
}

double Hix::QML::InputSpinBox::getValue() const
{
	return _value;
}

void Hix::QML::InputSpinBox::setValue(double value)
{
	_value = value;
	emit valueChanged();
}

Hix::QML::TextInputBox::TextInputBox(QQuickItem* parent) : QQuickItem(parent)
{
}

Hix::QML::TextInputBox::~TextInputBox()
{
}

std::string Hix::QML::TextInputBox::getInputText() const
{
	std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> utf16_to_utf8;
	return utf16_to_utf8.to_bytes(_text.toStdU16String());
}

void Hix::QML::TextInputBox::setInputText(std::string text)
{
	_text.fromStdString(text);
	emit inputTextChanged();
}

Hix::QML::DropdownBox::DropdownBox(QQuickItem* parent) : QQuickItem(parent)
{
}

Hix::QML::DropdownBox::~DropdownBox()
{
}

int Hix::QML::DropdownBox::getIndex() const
{
	return _index;
}

void Hix::QML::DropdownBox::setIndex(int index)
{
	_index = index;
	emit indexChanged();
}
