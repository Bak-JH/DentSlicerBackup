#include "FeatureManager.h"
#include "ApplicationManager.h"
#include "../Qml/util/QMLUtil.h"
#include "../Qml/components/Buttons.h"
#include "../Qml/components/FeatureMenu.h"
#include "../feature/FeaturesLoader.h"
#include "../glmodel.h"
#include "../feature/interfaces/Mode.h"
#include "../feature/settingMode.h"
#include "../feature/move.h"
#include "../feature/rotate.h"
#include "../feature/UndoRedo.h"
#include <qquickitem.h>

using namespace Hix::QML;
using namespace Hix::Features;
using namespace Hix::Application;
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

QQuickItem* Hix::Application::FeatureManager::slideArea()
{
	return _slideArea;
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

bool Hix::Application::FeatureManager::allowModelSelection() const
{
	return isActive<MoveMode>() || isActive<RotateMode>() || !Hix::Application::ApplicationManager::getInstance().featureManager().isFeatureActive();
}


void Hix::Application::FeatureManagerLoader::init(FeatureManager& manager, QObject* root)
{
	Hix::QML::getItemByID(root, manager._featureArea, "featureArea");
	Hix::QML::getItemByID(root, manager._slideArea, "slideArea");
	Hix::QML::getItemByID(root, manager._menu, "featureMenu");
	Hix::QML::getItemByID(root, manager._settingButton, "settingbutton");
	Hix::QML::getItemByID(root, manager._undoButton, "undo");
	Hix::QML::getItemByID(root, manager._redoButton, "redo");
	QObject::connect(manager._settingButton, &Hix::QML::Controls::Button::clicked, openFeatureModeFunctor<SettingMode>());
	QObject::connect(manager._undoButton, &Hix::QML::Controls::Button::clicked, [&manager]() {
		manager.setMode(nullptr);
		Hix::Application::ApplicationManager::getInstance().taskManager().enqueTask(new Hix::Features::Undo());
		});
	QObject::connect(manager._redoButton, &Hix::QML::Controls::Button::clicked, [&manager]() {
		manager.setMode(nullptr);
		Hix::Application::ApplicationManager::getInstance().taskManager().enqueTask(new Hix::Features::Redo());
		});
	//QObject::connect(manager._redoButton, &Hix::QML::Controls::Button::clicked, []() {&FeatureHistoryManager::redo});




}
