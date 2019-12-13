#include "ModalWindow.h"

void Hix::QML::ModalShell::modalmsgChanged()
{
	qDebug() << _modalmsg;
}

Hix::QML::ModalShell::ModalShell(QQuickItem* parent)
{
	setParent(parent);
}



