#include "ApplicationManager.h"
#include "PartManager.h"
#include "Qml/util/QMLUtil.h"
#include <Qt3DCore>
#include <qquickitem.h>
#include <deque>
#include <qqmlcontext.h>
#include "../Qml/components/PrintInfo.h"
#include "../input/raycastcontroller.h"
#include "../Qml/components/MainWindow.h"
//debug
#include <QDebug>
//for absolute correctness
#ifdef _DEBUG
//#define _STRICT_DEBUG
#include "common/debugging/DebugRenderObject.h"
#endif

using namespace Hix::Application;
using namespace Hix::QML;
using namespace Hix::Input;
using namespace Hix::Tasking;

Hix::Application::ApplicationManager::ApplicationManager(): _engine(new QQmlApplicationEngine()), _modalManager(_engine)
{
}

QQmlApplicationEngine& Hix::Application::ApplicationManager::engine()
{
	return *_engine;
}

void Hix::Application::ApplicationManager::init()
{
	_windowRoot = dynamic_cast<QQuickItem*>(_engine->rootObjects().first());
	QQuickItem* uiRoot;
	getItemByID(_windowRoot, _mainWindow, "window");
	getItemByID(_windowRoot, uiRoot, "uiRoot");
	//other singleton managers
	QQuickItem* modalItem;
	getItemByID(_windowRoot, modalItem, "dialogItem");
	FeatureManagerLoader::init(_featureManager, uiRoot);
	SceneManagerLoader::init(_sceneManager, uiRoot);
	PartManagerLoader::init(_partManager, uiRoot);
	ModalDialogManagerLoader::init(_modalManager, modalItem);
	RayCastControllerLoader::init(_rayCastController, _sceneManager.root());
	TaskManagerLoader::init(_taskManager, _engine);
	_supportRaftManager.initialize(_partManager.modelRoot());
	//settings
	_setting.parse();
	_updateChecker.init();
	//print info
	QQuickItem* printInfoQ;
	getItemByID(_windowRoot, printInfoQ, "printInfo");
	_printInfo = dynamic_cast<Hix::QML::PrintInfo*>(printInfoQ);
#ifdef _DEBUG
	Hix::Debug::DebugRenderObject::getInstance().initialize(_partManager.modelRoot());
#else
	//only auto update for release
	_updateChecker.checkForUpdates();

#endif
}
QQuickItem* Hix::Application::ApplicationManager::getWindowRoot() const
{
	return _windowRoot;
}


PartManager& Hix::Application::ApplicationManager::partManager()
{
	return _partManager;
}

ModalDialogManager& Hix::Application::ApplicationManager::modalDialogManager()
{
	return _modalManager;
}

SceneManager& Hix::Application::ApplicationManager::sceneManager()
{
	return _sceneManager;
}

FeatureManager& Hix::Application::ApplicationManager::featureManager()
{
	return _featureManager;
}

MouseCursorManager& Hix::Application::ApplicationManager::cursorManager()
{
	return _cursorManager;
}

I18nManager& Hix::Application::ApplicationManager::i18n()
{
	return _i18nManager;
}

Hix::Input::RayCastController& Hix::Application::ApplicationManager::getRayCaster()
{
	return _rayCastController;
}

const Hix::Settings::AppSetting& Hix::Application::ApplicationManager::settings() const
{
	return _setting;
}

Hix::Tasking::TaskManager& Hix::Application::ApplicationManager::taskManager()
{
	return _taskManager;
}

Hix::Support::SupportRaftManager& Hix::Application::ApplicationManager::supportRaftManager()
{
	return _supportRaftManager;
}

Hix::Auth::AuthManager& Hix::Application::ApplicationManager::auth()
{
	return _auth;
}

Hix::Utils::UpdateChecker& Hix::Application::ApplicationManager::updater()
{
	return _updateChecker;
}

QString Hix::Application::ApplicationManager::getVersion() const
{
	return QString::fromStdString(settings().deployInfo.version);
}


void Hix::Application::ApplicationManager::stateChanged()
{
	_printInfo->printVolumeChanged(_partManager.selectedModelsLengths());
}

Hix::Settings::AppSetting& Hix::Application::SettingsChanger::settings(Hix::Application::ApplicationManager& appMan)
{
	return appMan._setting;
}
