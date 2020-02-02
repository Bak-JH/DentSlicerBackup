#include "PopupShell.h"
#include "qmlmanager.h"

using namespace  Hix::QML;

Hix::QML::FeaturePopupShell::FeaturePopupShell(QQuickItem* parent) : QQuickItem(parent)
{
}

Hix::QML::FeaturePopupShell::~FeaturePopupShell()
{
}
void Hix::QML::FeaturePopupShell::componentComplete()
{
	registerOwningControls();
	getControl(_closeButton, "closeButton");
	getControl(_applyButton, "applyButton");

}
QQuickItem* Hix::QML::FeaturePopupShell::getQItem()
{
	return this;
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



Hix::QML::PartList::PartList(QQuickItem* parent) : QQuickItem(parent)
{
}

Hix::QML::PartList::~PartList()
{
}

void Hix::QML::PartList::appendModel(GLModel* model)
{
	qDebug() << "model: " << model;
	QObject* listModel = FindItemByName(qmlManager->engine, "modelList");
	QMetaObject::invokeMethod(listModel, "appendModel",
		Q_ARG(QVariant, model->modelName()), Q_ARG(QVariant, QVariant::fromValue<GLModel*>(model)));
}

Q_INVOKABLE void Hix::QML::PartList::deleteModels()
{
	QObject* listModel = FindItemByName(qmlManager->engine, "modelList");
	for (auto each : qmlManager->getSelectedModels())
		QMetaObject::invokeMethod(listModel, "deleteModel", Q_ARG(QVariant, QVariant::fromValue<GLModel*>(each)));;

	// delete code
	// blah blah blah
}
