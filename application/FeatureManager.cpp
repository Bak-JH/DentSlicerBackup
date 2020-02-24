#include "FeatureManager.h"
#include "ApplicationManager.h"
#include "../Qml/util/QMLUtil.h"
#include "../glmodel.h"
#include "../Qml/components/FeatureMenu.h"
#include "../feature/interfaces/Mode.h"
#include <qquickitem.h>

Hix::Application::FeatureManager::FeatureManager()
{
}


bool Hix::Application::FeatureManager::isFeatureActive()
{
	return _currentMode.get() != nullptr;
}


QQuickItem* Hix::Application::FeatureManager::featureArea()
{
	return _featureArea;
}

Hix::Features::Mode* Hix::Application::FeatureManager::currentMode() const
{
	return _currentMode.get();
}

void Hix::Application::FeatureManager::setMode(Hix::Features::Mode* mode)
{
	_currentMode.reset(mode);
	auto instantMode = dynamic_cast<Hix::Features::InstantMode*>(mode);
	if (instantMode)
	{
		_currentMode.reset(nullptr);
	}
}

Hix::Features::FeatureHistoryManager& Hix::Application::FeatureManager::featureHistoryManager()
{
	return _featureHistoryManager;
}




void Hix::Application::FeatureManagerLoader::init(FeatureManager& manager, QObject* root)
{
	Hix::QML::getItemByID(root, manager._featureArea, "featureArea");
}
