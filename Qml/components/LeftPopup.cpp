#include "LeftPopup.h"
#include "qmlmanager.h"

using namespace  Hix::QML;

Hix::QML::LeftPopupShell::LeftPopupShell(QQuickItem* parent) : QQuickItem(parent)
{
	
}

Hix::QML::ProgressPopupShell::ProgressPopupShell(QQuickItem* parent) : QQuickRectangle(parent)
{
}

Hix::QML::ProgressPopupShell::~ProgressPopupShell()
{
}

Q_INVOKABLE void Hix::QML::ProgressPopupShell::appendFeature(QString featureName)
{
	QObject* listModel = FindItemByName(qmlManager->engine, "model");
	QMetaObject::invokeMethod(listModel, "appendFeature", Q_ARG(QVariant, featureName));
}
