#include "ApplicationManager.h"
#include "PartManager.h"
#include "Qml/util/QMLUtil.h"
#include <Qt3DCore>
#include <qquickitem.h>
#include <deque>
#include <qqmlcontext.h>
#include "../Qml/components/PrintInfo.h"
using namespace Hix::Application;
using namespace Hix::QML;
template<typename QType>
void findItem(QObject* start, QType*& found, const QString& id)
{
	std::unordered_map<std::string, QType*> map;
	std::deque<QObject*> s;
	s.emplace_back(start);

	while (!s.empty())
	{
		auto curr = s.front();
		s.pop_front();

		auto neighbors = curr->children();
		for (auto& each : neighbors)
		{
			auto inputControl = dynamic_cast<QObject*>(each);
			if (inputControl)
			{
				auto context = qmlContext(each);
				if (context && context->nameForObject(curr) == id)
				{
					found = dynamic_cast<QType*>(curr);
					return;
				}
			}
			s.emplace_back(each);
		}
	}
	throw std::runtime_error("findItem failed");
}


QQmlApplicationEngine& Hix::Application::ApplicationManager::engine()
{
	return _engine;
}

void Hix::Application::ApplicationManager::init()
{
	//auto root = _engine.rootObjects().first();
	//findItem(root, _uiRoot, "uiRoot");
	//findItem(root, _sceneRoot, "sceneRoot");
	//Qt3DCore::QEntity* partRoot = nullptr;
	//findItem(_sceneRoot, partRoot, "models");
	//QQuickItem* modalItem;
	//findItem(root, modalItem, "dialogItem");
	//PartManagerLoader::init(_partManager, partRoot);
	//ModalDialogManagerLoader::init(_modalManager, modalItem);
	//QQuickItem* printInfoQ;
	//findItem(root, printInfoQ, "printinfo");
	_windowRoot = dynamic_cast<QQuickItem*>(_engine.rootObjects().first());

	getItemByID(_windowRoot, _uiRoot, "uiRoot");
	getItemByID(_windowRoot, _sceneRoot, "sceneRoot");
	getItemByID(_windowRoot, _entityRoot, "total");
	Qt3DCore::QEntity* partRoot = nullptr;
	getItemByID(_sceneRoot, partRoot, "models");
	QQuickItem* modalItem;
	getItemByID(_windowRoot, modalItem, "dialogItem");
	PartManagerLoader::init(_partManager, partRoot);
	ModalDialogManagerLoader::init(_modalManager, modalItem);
	QQuickItem* printInfoQ;
	getItemByID(_windowRoot, printInfoQ, "printinfo");

	_printInfo = dynamic_cast<Hix::QML::PrintInfo*>(printInfoQ);

}

QQuickItem* Hix::Application::ApplicationManager::getUIRoot()const
{
	return _uiRoot;
}

QQuickItem* Hix::Application::ApplicationManager::getWindowRoot() const
{
	return _windowRoot;
}

Qt3DCore::QEntity* Hix::Application::ApplicationManager::getEntityRoot() const
{
	return _entityRoot;
}

PartManager& Hix::Application::ApplicationManager::partManager()
{
	return _partManager;
}

ModalDialogManager& Hix::Application::ApplicationManager::modalDialogManager()
{
	return _modalManager;
}

void Hix::Application::ApplicationManager::stateChanged()
{
	_printInfo->printVolumeChanged(_partManager.selectedModelsLengths());
}
