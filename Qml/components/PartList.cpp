#include "PartList.h"
#include "Buttons.h"
#include "../util/QMLUtil.h"
#include "glmodel.h"
#include "Buttons.h"
#include <QMetaObject>

#include "../../feature/deleteModel.h"

#include "../application/ApplicationManager.h"
#include "../feature/FeaturesLoader.h"
#include "../feature/deleteModel.h"
using namespace Hix::QML;
using namespace Hix::Features;
const QUrl ITEM_URL = QUrl("qrc:/Qml/PartListItem.qml");

Hix::QML::PartList::PartList(QQuickItem* parent) : QQuickItem(parent), _component(&Hix::Application::ApplicationManager::getInstance().getInstance().engine(), ITEM_URL)
{
#ifdef _DEBUG
	if (!_component.isReady())
	{
		qDebug() << "_component error: " << _component.errors();
	}
#endif
}

Hix::QML::PartList::~PartList()
{
}

void Hix::QML::PartList::listModel(GLModel* model)
{
	auto item = dynamic_cast<Hix::QML::PartListItem*>(_component.create(qmlContext(this)));
	item->setTargetModel(model);
	item->setParentItem(_itemContainer);
	_items.emplace(model, item);
	//event handler
	//QObject::connect(item->selectButton(), &Hix::QML::Controls::ToggleSwitch::checked, [this, model]() {
	//	////unselect previously selected parts if multi selection is not active.
	//	//if (!Hix::Application::ApplicationManager::getInstance().partManager().isMultiSelect())
	//	//{
	//	//	unselectAll(false);
	//	//}
	//	setModelSelected(model, true, false);
	//	});
	//QObject::connect(item->selectButton(), &Hix::QML::Controls::ToggleSwitch::unchecked, [this, model]() {
	//	setModelSelected(model, false, false);
	//	});

	QObject::connect(item->selectButton(), &Hix::QML::Controls::Button::clicked, [this, model]() {
		if (Hix::Application::ApplicationManager::getInstance().partManager().isContiguousSelect()) {
			setContiguousSelected(model);
		}
		else {
			model->modelSelectionClick();
		}
		});
	QObject::connect(item->hideButton(), &Hix::QML::Controls::ToggleSwitch::checked, [this, model]() {
		model->setEnabled(false);
		});
	QObject::connect(item->hideButton(), &Hix::QML::Controls::ToggleSwitch::unchecked, [this, model]() {
		model->setEnabled(true);
		});

}

bool Hix::QML::PartList::unlistModel(GLModel* model)
{
	setModelSelected(model, false);
	_selectedModels.erase(model);
	if (isListed(model))
	{
		_items.erase(model);
		return true;
	}
	else
		return false;
}

bool Hix::QML::PartList::isListed(GLModel* model) const
{
	return _items.find(model) != _items.end();
}

bool Hix::QML::PartList::isSelected(GLModel* model)const
{
	return _selectedModels.find(model) != _selectedModels.end();
}

void Hix::QML::PartList::unselectAll()
{
	for (auto& each : _items)
	{
		auto model = each.first;
		if (isSelected(model))
		{
			setModelSelected(model, false);
		}
	}
}

void Hix::QML::PartList::selectAll()
{
	for (auto& each : _items)
	{
		auto model = each.first;
		if (!isSelected(model))
		{
			setModelSelected(model, true);
		}
	}
}

bool Hix::QML::PartList::setModelSelected(GLModel* model, bool isSelected)
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
			model->updatePrintable();
			return true;
		}
	}
	return false;
}

bool Hix::QML::PartList::setContiguousSelected(GLModel* model)
{
	auto lastSelectedIdx = _itemContainer->childItems().indexOf(_items.find(*--_selectedModels.end())->second.get());
	auto clickedIdx = _itemContainer->childItems().indexOf(_items.find(model)->second.get());

	for (auto idx = std::max(lastSelectedIdx, clickedIdx); idx >= std::min(lastSelectedIdx, clickedIdx); --idx)
	{
		auto target = dynamic_cast<PartListItem*>(_itemContainer->childItems().at(idx))->targetModel();
		setModelSelected(target, true);
	}

	return true;
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
	emit _deleteButton->clicked();
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
	if (selected)
	{
		QMetaObject::invokeMethod(_selectButton, "setSelected");
	}
	else
	{
		QMetaObject::invokeMethod(_selectButton, "setUnselected");

	}

}

void Hix::QML::PartListItem::setTargetModel(GLModel* model)
{
	_targetModel = model;
	_modelName = model->modelName();
	emit nameChanged();
}

GLModel* Hix::QML::PartListItem::targetModel()
{
	return _targetModel;
}

bool Hix::QML::PartListItem::visible() const
{
	return !_hideButton->isChecked();
}

Hix::QML::Controls::ToggleSwitch* Hix::QML::PartListItem::hideButton()
{
	return _hideButton;
}

Hix::QML::Controls::Button* Hix::QML::PartListItem::selectButton()
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
