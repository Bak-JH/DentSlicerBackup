#pragma once
#include "../common/Singleton.h"
#include <QQmlApplicationEngine>
#include "PartManager.h"
#include "ModalDialogManager.h"
#include "SceneManager.h"
#include "I18nManager.h"
#include "FeatureManager.h"
#include "../input/raycastcontroller.h"
#include "../Settings/AppSetting.h"
#include "../Tasking/TaskManager.h"
#include "../support/SupportRaftManager.h"
#include "MouseCursorManager.h"
#include "../auth/AuthManager.h"
#include "utils/updatechecker.h"
#include "../utils/updatechecker.h"
class QQuickItem;
namespace Qt3DCore
{
	class QEntity;
}
namespace Hix
{


	namespace Auth
	{
		class AuthManager;
	}
	namespace Features
	{
		class SettingMode;
		class SupportMode;
		class SliceExportMode;

	}
	namespace QML
	{
		class MainWindow;
		class PrintInfo;
	}
	namespace Application
	{
		class ApplicationManager: public Hix::Common::Singleton<ApplicationManager>
		{
		public:
			ApplicationManager();
			ApplicationManager(const ApplicationManager& other) = delete;
			ApplicationManager(ApplicationManager&& other) = delete;
			ApplicationManager& operator=(ApplicationManager other) = delete;
			ApplicationManager& operator=(ApplicationManager&& other) = delete;

			QQmlApplicationEngine& engine();
			void init();

			//TODO: temp
			void stateChanged();
			//TODO: temp end

			QQuickItem* getWindowRoot()const;
			PartManager& partManager();
			ModalDialogManager& modalDialogManager();
			SceneManager& sceneManager();
			FeatureManager& featureManager();
			MouseCursorManager& cursorManager();
			I18nManager& i18n();
			Hix::Input::RayCastController& getRayCaster();
			const Hix::Settings::AppSetting& settings()const;
			Hix::Tasking::TaskManager& taskManager();
			Hix::Support::SupportRaftManager& supportRaftManager();
			Hix::Auth::AuthManager& auth();
			Hix::Utils::UpdateChecker& updater();

			QString getVersion()const;

		private:
			QQmlApplicationEngine* _engine;
			PartManager _partManager;
			ModalDialogManager _modalManager;
			SceneManager _sceneManager;
			FeatureManager _featureManager;
			MouseCursorManager _cursorManager;
			I18nManager _i18nManager;
			Hix::Utils::UpdateChecker _updateChecker;
			Hix::Input::RayCastController _rayCastController;
			Hix::Tasking::TaskManager _taskManager;
			Hix::Support::SupportRaftManager _supportRaftManager;
			Hix::Settings::AppSetting _setting;
			Hix::Auth::AuthManager _auth;
			//TODO:temp legacy
			//root of entire window application ie) including login window
			QQuickItem* _windowRoot;
			Hix::QML::MainWindow* _mainWindow;

			Hix::QML::PrintInfo* _printInfo;
			friend class SettingsChanger;

		};



		class SettingsChanger
		{
		private:
			static Hix::Settings::AppSetting& settings(Hix::Application::ApplicationManager& appMan);
			friend class Hix::Features::SettingMode;
			friend class Hix::Features::SupportMode;
			friend class Hix::Settings::BasicSetting;
			friend class Hix::Features::SliceExportMode;
			friend class Hix::Auth::AuthManager;
			friend class Hix::Utils::UpdateChecker;


		};


	}
}
