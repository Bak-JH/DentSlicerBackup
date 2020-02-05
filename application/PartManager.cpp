#include "PartManager.h"
#include "ApplicationManager.h"
#include "../Qml/components/PartList.h"
#include "../Qml/util/QMLUtil.h"
Hix::Application::PartManager::PartManager()
{
}

void Hix::Application::PartManager::addPart(std::unique_ptr<GLModel>&& model)
{
	auto raw = model.get();
	auto find = _models.find(raw);
	if (find == _models.end())
	{
		_models.emplace(std::make_pair(raw, model));
	}
}


std::unique_ptr<GLModel> Hix::Application::PartManager::removePart(GLModel* model)
{
	auto node = _models.extract(model);
	return std::unique_ptr<GLModel>(std::move(node.mapped()));
}

void Hix::Application::PartManager::isSelected(GLModel* model)
{

}

void Hix::Application::PartManager::selectPart(GLModel* model)
{
}

void Hix::Application::PartManager::selectAll()
{
}

void Hix::Application::PartManager::unselectAll()
{
}

std::unordered_set<GLModel*> Hix::Application::PartManager::selectedModels() const
{
	return std::unordered_set<GLModel*>();
}

std::unordered_set<GLModel*> Hix::Application::PartManager::allModels() const
{
	return std::unordered_set<GLModel*>();
}

Hix::Engine3D::Bounds3D Hix::Application::PartManager::selectedBound() const
{
	return Hix::Engine3D::Bounds3D();
}

QVector3D Hix::Application::PartManager::getSelectedCenter() const
{
	return QVector3D();
}

QVector3D Hix::Application::PartManager::selectedModelsLengths() const
{
	return QVector3D();
}

void Hix::Application::PartManagerLoader::init(PartManager& manager, Qt3DCore::QEntity* entity)
{
	manager._root = entity;
	Hix::QML::getItemByID(Hix::Application::ApplicationManager::getInstance().getUIRoot(), manager._partList, "partList");

}
