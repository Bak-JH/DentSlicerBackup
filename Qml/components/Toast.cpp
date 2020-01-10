#include "Toast.h"

void Hix::QML::ToastShell::toastmsgChanged()
{
	qDebug() << _toastmsg;
}

Hix::QML::ToastShell::ToastShell(QQuickItem* parent)
{
	setParent(parent);
}

