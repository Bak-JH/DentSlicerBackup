#pragma once
#include "../common/Singleton.h"
#include <QQmlApplicationEngine>
#include "PartManager.h"
#include "ModalDialogManager.h"
#include "SceneManager.h"
#include "FeatureManager.h"
#include "../input/raycastcontroller.h"
#include "../input/KeyboardController.h"

#include "../Settings/AppSetting.h"
#include "../Tasking/TaskManager.h"
#include "../support/SupportRaftManager.h"
#include "MouseCursorManager.h"
class QQuickItem;
namespace Qt3DCore
{
	class QEntity;
}
namespace Hix
{
	namespace Features
	{
		class SettingMode;
		class SupportMode;
		class SliceExportMode;

	}
	namespace QML
	{
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
			Hix::Input::RayCastController& getRayCaster();
			const Hix::Settings::AppSetting& settings()const;
			Hix::Tasking::TaskManager& taskManager();
			Hix::Support::SupportRaftManager& supportRaftManager();
			QString getVersion()const;

		private:
			QQmlApplicationEngine _engine;
			PartManager _partManager;
			ModalDialogManager _modalManager;
			SceneManager _sceneManager;
			FeatureManager _featureManager;
			MouseCursorManager _cursorManager;
			Hix::Input::RayCastController _rayCastController;
			Hix::Input::KeyboardController _keyboardController;
			Hix::Tasking::TaskManager _taskManager;
			Hix::Support::SupportRaftManager _supportRaftManager;
			Hix::Settings::AppSetting _setting;

			//TODO:temp legacy
			//root of entire window application ie) including login window
			QQuickItem* _windowRoot;
			Hix::QML::PrintInfo* _printInfo;
			friend class SettingsChanger;
		};



		class SettingsChanger
		{
		private:
			static Hix::Settings::AppSetting& settings(Hix::Application::ApplicationManager& appMan);
			friend class Hix::Features::SettingMode;
			friend class Hix::Features::SupportMode;
			friend class Hix::Features::SliceExportMode;


		};


	}
}
