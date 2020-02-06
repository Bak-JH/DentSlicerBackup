#include "PopupShell.h"
#include "Buttons.h"
#include <QtQuick/private/qquickrectangle_p.h>
#include <QtQuick/private/qquicktext_p.h>
#include "QtQml/private/qqmllistmodel_p.h"

#include "glmodel.h"
using namespace  Hix::QML;

Hix::QML::FeaturePopupShell::FeaturePopupShell(QQuickItem* parent) : QQuickItem(parent)
{
	qDebug() << "created feature";
}

Hix::QML::FeaturePopupShell::~FeaturePopupShell()
{
	qDebug() << "deleted feature";
}
void Hix::QML::FeaturePopupShell::componentComplete()
{
	__super::componentComplete();
	registerOwningControls();
	getControl(_closeButton, "closeButton");
	getControl(_applyButton, "applyButton");

}
QQuickItem* Hix::QML::FeaturePopupShell::getQItem()
{
	return this;
}

void Hix::QML::FeaturePopupShell::setApplyReady(bool isReady)
{
	_applyReady = isReady;
}

bool Hix::QML::FeaturePopupShell::getApplyReady() const
{
	return _applyReady;
}

Controls::Button& Hix::QML::FeaturePopupShell::closeButton()
{
	return *_closeButton;
}

Controls::Button& Hix::QML::FeaturePopupShell::applyButton()
{
	return *_applyButton;
}


Hix::QML::ProgressPopupShell::ProgressPopupShell(QQuickItem* parent) : QQuickItem(parent)
{
}

Hix::QML::ProgressPopupShell::~ProgressPopupShell()
{
}

void Hix::QML::ProgressPopupShell::appendFeature(std::string featureName)
{
	QObject* listModel = FindItemByName(qmlManager->engine, "featureList");
	QMetaObject::invokeMethod(listModel, "appendFeature", Q_ARG(QVariant, QString::fromStdString(featureName)));
}

void Hix::QML::ProgressPopupShell::appendFeatureList(QStringList featureList)
{
	for (auto each : featureList)
	{
		appendFeature(each.toStdString());
	}
}

void Hix::QML::ProgressPopupShell::setPercentage(int percent)
{
	_percent = percent;
	emit percentChanged();
}



Hix::QML::ModalShell::ModalShell(QQuickItem* parent) : QQuickItem(parent)
{
}

Hix::QML::ModalShell::~ModalShell()
{
}

void Hix::QML::ModalShell::setMessage(std::string message)
{
	_modalmsg = QString::fromStdString(message);
	emit modalmsgChanged();
}



