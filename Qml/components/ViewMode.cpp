#include "ViewMode.h"

Hix::QML::ViewModeShell::ViewModeShell(QQuickItem* parent) : QQuickItem(parent)
{
}

void Hix::QML::ViewModeShell::viewtypeChanged()
{
	qDebug() << _viewtype;
}

