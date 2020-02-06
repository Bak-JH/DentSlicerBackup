#include "PartList.h"
#include "Buttons.h"
#include <QtQuick/private/qquickrectangle_p.h>
#include <QtQuick/private/qquicktext_p.h>
#include "QtQml/private/qqmllistmodel_p.h"
#include "../util/QMLUtil.h"
#include "glmodel.h"
#include "Buttons.h"
#include "../../qmlmanager.h"
#include "../application/ApplicationManager.h"
using namespace  Hix::QML;


Hix::QML::PartList::PartList(QQuickItem* parent) : QQuickItem(parent)
{
}

Hix::QML::PartList::~PartList()
{
}

void Hix::QML::PartList::listModel(GLModel* model)
{
	QMetaObject::invokeMethod(this, "appendModel",
		Q_ARG(QVariant, model->modelName()), Q_ARG(QVariant, QVariant::fromValue<GLModel*>(model)));
}

bool Hix::QML::PartList::isSelected(GLModel* model)const
{
	return _selectedModels.find(model) != _selectedModels.end();
}

void Hix::QML::PartList::selectPart(GLModel* model)
{
	auto 
}

void Hix::QML::PartList::unselectPart(GLModel* model)
{
}

std::unordered_set<GLModel*> Hix::QML::PartList::selectedModels() const
{
	return _selectedModels;
}

Hix::Engine3D::Bounds3D Hix::QML::PartList::selectedBound() const
{
	return Hix::Engine3D::combineBounds(_selectedModels);
}

void Hix::QML::PartList::deleteSelectedModels()
{
	//run delete feature
}
QQuickItem* Hix::QML::PartList::getQItem()
{
	return this;
}

void Hix::QML::PartList::componentComplete()
{
	__super::componentComplete();
	registerOwningControls();
	getControl(_deleteButton, "deleteButton");
}	


QQuickItem* Hix::QML::PartListItem::getQItem()
{
	return this;
}


Hix::QML::PartListItem::PartListItem(QQuickItem* parent) : QQuickItem(parent)
{

}


Hix::QML::PartListItem::~PartListItem()
{
}

void Hix::QML::PartListItem::componentComplete()
{
	__super::componentComplete();
	registerOwningControls();
	getControl(_hideButton, "hideButton");
	getControl(_selectButton, "selectButton");
	_model = this->property("modelPointer").value<GLModel*>();

}
