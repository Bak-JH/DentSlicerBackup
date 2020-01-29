#include "PartList.h"
#include "qmlmanager.h"

Hix::QML::PartList::PartList(QQuickItem* parent) : QQuickItem(parent)
{
}

Hix::QML::PartList::~PartList()
{
}

Q_INVOKABLE void Hix::QML::PartList::appendModel(QString modelName)
{
	QObject* listModel = FindItemByName(qmlManager->engine, "modelList");
	QMetaObject::invokeMethod(listModel, "appendModel", Q_ARG(QVariant, modelName));;
}



Hix::QML::PartDeleteButton::PartDeleteButton(QQuickItem* parent) : _mouseArea(new QQuickMouseArea(this))
{
	setParent(parent);
	connect(_mouseArea, &QQuickMouseArea::clicked, this, &PartDeleteButton::onClick);
	qvariant_cast<QObject*>(
		_mouseArea->property("anchors")
		)->setProperty("fill", _mouseArea->property("parent"));

}

Hix::QML::PartDeleteButton::~PartDeleteButton()
{
}

void Hix::QML::PartDeleteButton::onClick()
{
	qDebug() << "part clicked";
}