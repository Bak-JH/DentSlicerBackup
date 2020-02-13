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
#include "../feature/FeaturesLoader.h"
#include "../feature/deleteModel.h"
using namespace Hix::QML;
using namespace Hix::Features;
const QUrl ITEM_URL = QUrl("qrc:/Qml/PartListItem.qml");

Hix::QML::PartList::PartList(QQuickItem* parent) : QQuickItem(parent), _component(&Hix::Application::ApplicationManager::getInstance().getInstance().engine(), ITEM_URL)
{
}

Hix::QML::PartList::~PartList()
{
}

void Hix::QML::PartList::listModel(GLModel* model)
{
	auto item = dynamic_cast<Hix::QML::PartListItem*>(_component.create(qmlContext(this)));
	item->setParentItem(_itemContainer);
	_items.emplace(model, item);
	//event handler
	QObject::connect(item->selectButton(), &Hix::QML::Controls::ToggleSwitch::checked, [this, model]() {
		setModelSelected(model, true);
		});
	QObject::connect(item->selectButton(), &Hix::QML::Controls::ToggleSwitch::unchecked, [this, model]() {
		setModelSelected(model, false);
		});
	QObject::connect(item->hideButton(), &Hix::QML::Controls::ToggleSwitch::checked, [this, model]() {
		model->setEnabled(false);
		});
	QObject::connect(item->hideButton(), &Hix::QML::Controls::ToggleSwitch::unchecked, [this, model]() {
		model->setEnabled(true);
		});

}

void Hix::QML::PartList::unlistModel(GLModel* model)
{
	_selectedModels.erase(model);
	_items.erase(model);
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


QQuickItem* Hix::QML::PartList::getQItem()
{
	return this;
}

void Hix::QML::PartList::componentComplete()
{
	__super::componentComplete();
	registerOwningControls();
	getItemByID(this, _itemContainer, "itemContainer");
	getControl(_deleteButton, "deleteButton");
	QObject::connect(_deleteButton, &Hix::QML::Controls::Button::clicked, Hix::Features::openFeatureModeFunctor<DeleteModelMode>(nullptr));
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

Hix::QML::Controls::ToggleSwitch* Hix::QML::PartListItem::hideButton()
{
	return _hideButton;
}

Hix::QML::Controls::ToggleSwitch* Hix::QML::PartListItem::selectButton()
{
	return _selectButton;
}

void Hix::QML::PartListItem::componentComplete()
{
	__super::componentComplete();
	registerOwningControls();
	getControl(_hideButton, "hideButton");
	getControl(_selectButton, "selectButton");

}
