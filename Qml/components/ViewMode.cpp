#include "ViewMode.h"

using namespace Hix::QML;

Hix::QML::ViewModeShell::ViewModeShell(QQuickItem* parent) : QQuickItem(parent)
{
}

Hix::QML::ViewModeShell::~ViewModeShell()
{
}

Q_INVOKABLE void Hix::QML::ViewModeShell::setViewType(ViewType type)
{
	_viewtype = type;
	emit viewtypeChanged();
}

ViewModeShell::ViewType Hix::QML::ViewModeShell::getViewType() const
{
	return _viewtype;
}
