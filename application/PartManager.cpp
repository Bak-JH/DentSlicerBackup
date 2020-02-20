#include "PartManager.h"
#include "ApplicationManager.h"
#include "../Qml/components/PartList.h"
#include "../Qml/util/QMLUtil.h"
#include "../Qml/components/PrintInfo.h"
#include "../glmodel.h"
#include <qquickitem.h>
Hix::Application::PartManager::PartManager()
{
}

void Hix::Application::PartManager::addPart(std::unique_ptr<GLModel>&& model)
{
	auto raw = model.get();
	auto find = _models.find(raw);
	if (find == _models.end())
	{
		_models.emplace(raw, std::move(model));
		//list model
		_partList->listModel(raw);
	}
}

std::unique_ptr<GLModel> Hix::Application::PartManager::removePart(GLModel* model)
{
	_partList->unlistModel(model);
	auto node = _models.extract(model);
	return std::unique_ptr<GLModel>(std::move(node.mapped()));
}

bool Hix::Application::PartManager::isTopLevel(GLModel* model) const
{
	return _models.find(model) != _models.end();
}

bool Hix::Application::PartManager::isSelected(GLModel* model)const
{
	return _partList->isSelected(model);
}

void Hix::Application::PartManager::setSelected(GLModel* model, bool selected)
{
	if (_partList->setModelSelected(model, selected))
	{
		model->updatePrintable();
	}
}


void Hix::Application::PartManager::selectAll()
{
	for (auto& each : _models)
	{
		auto modelRaw = each.first;
		if (!_partList->isSelected(modelRaw))
		{
			_partList->setModelSelected(modelRaw, true);
		}
	}
}

void Hix::Application::PartManager::unselectAll()
{
	for (auto& each : _models)
	{
		auto modelRaw = each.first;
		if (_partList->isSelected(modelRaw))
		{
			_partList->setModelSelected(modelRaw, false);
		}
	}
}

std::unordered_set<GLModel*> Hix::Application::PartManager::selectedModels() const
{
	return _partList->selectedModels();
}

std::unordered_set<GLModel*> Hix::Application::PartManager::allModels() const
{
	std::unordered_set<GLModel*> allModels;
	std::transform(_models.begin(), _models.end(), std::inserter(allModels, allModels.begin()), [](auto& pair)->GLModel* {
		return pair.first;
		});
	return allModels;
}

Hix::Engine3D::Bounds3D Hix::Application::PartManager::selectedBound() const
{
	return Hix::Engine3D::combineBounds(selectedModels());
}

QVector3D Hix::Application::PartManager::getSelectedCenter() const
{
	return selectedBound().centre();
}

QVector3D Hix::Application::PartManager::selectedModelsLengths() const
{
	return selectedBound().lengths();
}

Qt3DCore::QEntity* Hix::Application::PartManager::modelRoot()
{
	return _root;
}

void Hix::Application::PartManagerLoader::init(PartManager& manager, QObject* root)
{
	Hix::QML::getItemByID(root, manager._root, "models");
	Hix::QML::getItemByID(root, manager._partList, "partList");
}
                                                                                                                       