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

int Hix::QML::InputSpinBox::getValue() const
{
	return _value;
}

void Hix::QML::InputSpinBox::setValue(int value)
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

Hix::QML::DropdownBox::DropdownBox(QQuickItem* parent)
{
}

Hix::QML::DropdownBox::~DropdownBox()
{
}