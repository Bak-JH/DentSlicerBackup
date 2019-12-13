#include "ViewMode.h"

Hix::QML::ViewModeShell::ViewModeShell(QQuickItem* parent)
{
	setParent(parent);
}

void Hix::QML::ViewModeShell::viewtypeChanged()
{
	qDebug() << _viewtype;
}

