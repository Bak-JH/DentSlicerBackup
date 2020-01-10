#include "SlideBar.h"


Hix::QML::SlideBarShell::SlideBarShell(QQuickItem* parent) :_slideRect(new QQuickSlider(this))
{
	setParent(parent);
}
