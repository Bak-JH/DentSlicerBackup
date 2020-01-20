#include "Toast.h"

Hix::QML::ToastShell::ToastShell(QQuickItem* parent) : QQuickItem(parent)
{
}

Hix::QML::ToastShell::~ToastShell()
{
}

void Hix::QML::ToastShell::setMessage(MessageType type, std::string text)
{
	_msgType = type;
	_message = QString::fromStdString(text);

	emit messageChanged();
	emit messageTypeChanged();
}
