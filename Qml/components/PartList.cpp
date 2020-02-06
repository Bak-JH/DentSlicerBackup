#include "PartList.h"
#include "Buttons.h"
#include <QtQuick/private/qquickrectangle_p.h>
#include <QtQuick/private/qquicktext_p.h>
#include "QtQml/private/qqmllistmodel_p.h"
#include "../util/QMLUtil.h"
#include "glmodel.h"
#include "Buttons.h"

#include "../../feature/deleteModel.h"
#include "../../qmlmanager.h"
#include "../application/ApplicationManager.h"
using namespace  Hix::QML;

class Hix::QML::PartListItemAttorny
{
private:
	static void itemAdded(PartList* parent, GLModel* model, PartListItem* item);
	friend class PartListItem;
};

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

bool Hix::QML::PartList::isListed(GLModel* model) const
{
	return _items.find(model) != _items.end();
}

bool Hix::QML::PartList::isSelected(GLModel* model)const
{
	return _selectedModels.find(model) != _selectedModels.end();
}

void Hix::QML::PartList::setModelSelected(GLModel* model, bool isSelected)
{
	auto listed = _items.find(model);
	if (listed != _items.end())
	{
		auto result = _selectedModels.find(model);
		auto alreadySelected = result != _selectedModels.end();
		if (isSelected != alreadySelected)
		{
			if (isSelected)
				_selectedModels.insert(model);
			else
				_selectedModels.erase(model);
			listed->second->setSelected(isSelected);
		}
	}
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
	_selectedModels.clear();

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

void Hix::QML::PartListItem::setSelected(bool selected)
{
	if (isSelected() != selected)
	{
		if (selected)
		{
			emit _selectButton->checked();
		}
		else
		{
			emit _selectButton->unchecked();
		}
	}
}

bool Hix::QML::PartListItem::isSelected() const
{
	return _selectButton->isChecked();
}



bool Hix::QML::PartListItem::visible() const
{
	return !_hideButton->isChecked();
}

void Hix::QML::PartListItem::componentComplete()
{
	__super::componentComplete();
	registerOwningControls();
	getControl(_hideButton, "hideButton");
	getControl(_selectButton, "selectButton");
	getParentByID(this, _parent, "partList");
	_model = this->property("modelPointer").value<GLModel*>();
	PartListItemAttorny::itemAdded(_parent, _model, this);
	QObject::connect(_selectButton, &Hix::QML::Controls::ToggleSwitch::checked, [this]() {
		_parent->setModelSelected(this->_model, true);
		});
	QObject::connect(_selectButton, &Hix::QML::Controls::ToggleSwitch::unchecked, [this]() {
		_parent->setModelSelected(this->_model, false);
		});
	QObject::connect(_hideButton, &Hix::QML::Controls::ToggleSwitch::checked, [this]() {
		_model->setEnabled(false);
		});
	QObject::connect(_hideButton, &Hix::QML::Controls::ToggleSwitch::unchecked, [this]() {
		_model->setEnabled(true);
		});
}

void Hix::QML::PartListItemAttorny::itemAdded(PartList* parent, GLModel* model, PartListItem* item)
{
	parent->_items.emplace(model, item);
}
