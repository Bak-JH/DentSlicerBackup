#include "PartList.h"
#include "qmlmanager.h"
#include <QVariant>

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

//void Hix::QML::PartList::deleteModels(std::unordered_set<GLModel*>& selected)
//{
//	QObject* listModel = FindItemByName(qmlManager->engine, "modelList");
//
//	for (auto each : selected)
//		QMetaObject::invokeMethod(listModel, "deleteModel", Q_ARG(QVariant, QVariant::fromValue<GLModel*>(each)));;
//
//	// delete code
//	// blah blah blah
//}

Q_INVOKABLE void Hix::QML::PartList::deleteModels()
{
	QObject* listModel = FindItemByName(qmlManager->engine, "modelList");

	for (auto each : qmlManager->getSelectedModels())
		QMetaObject::invokeMethod(listModel, "deleteModel", Q_ARG(QVariant, QVariant::fromValue<GLModel*>(each)));;

	// delete code
	// blah blah blah
}