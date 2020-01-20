#include "PopupShell.h"
#include "qmlmanager.h"

using namespace  Hix::QML;

Hix::QML::LeftPopupShell::LeftPopupShell(QQuickItem* parent) : QQuickItem(parent)
{
}

Hix::QML::LeftPopupShell::~LeftPopupShell()
{
}

Hix::QML::ProgressPopupShell::ProgressPopupShell(QQuickItem* parent) : QQuickRectangle(parent)
{
}

Hix::QML::ProgressPopupShell::~ProgressPopupShell()
{
}

void Hix::QML::ProgressPopupShell::appendFeature(std::string featureName)
{
	QObject* listModel = FindItemByName(qmlManager->engine, "featueList");
	QMetaObject::invokeMethod(listModel, "appendFeature", Q_ARG(QVariant, QString::fromStdString(featureName)));
}

void Hix::QML::ProgressPopupShell::appendFeatureList(QStringList featureList)
{
	for (auto each : featureList)
	{
		appendFeature(each.toStdString());
	}
}
