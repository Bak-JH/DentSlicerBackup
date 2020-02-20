#pragma once
#include "../common/Singleton.h"
#include <QQmlApplicationEngine>
#include "PartManager.h"
#include "ModalDialogManager.h"
#include "SceneManager.h"
#include "../input/raycastcontroller.h"
#include "../Settings/AppSetting.h"
#include "../Tasking/TaskManager.h"
#include "../slice/SlicingOptBackend.h"
#include "../support/SupportRaftManager.h"

class QQuickItem;
namespace Qt3DCore
{
	class QEntity;
}
namespace Hix
{

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
			QQuickItem* getUIRoot()const;
			QQuickItem* getWindowRoot()const;
			QQuickItem* getScene3D()const;

			//TODO: temp
			Qt3DCore::QEntity* getEntityRoot()const;
			void stateChanged();
			//TODO: temp end


			PartManager& partManager();
			ModalDialogManager& modalDialogManager();
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
			Hix::Input::RayCastController _rayCastController;
			Hix::Tasking::TaskManager _taskManager;
			Hix::Support::SupportRaftManager _supportRaftManager;

			Hix::Settings::AppSetting _setting;

			//TODO:temp legacy
			SlicingOptBackend _optBackend;



			//root for all mesh entities including 3D uis TODO:TEMP
			Qt3DCore::QEntity* _entityRoot;
			QQuickItem* _scene3D;

			//root container of qquickitems in the MAIN application
			QQuickItem* _uiRoot;
			//root of entire window application ie) including login window
			QQuickItem* _windowRoot;
			//place where feature popup appears
			QQuickItem* _featureArea;
			Hix::QML::PrintInfo* _printInfo;
		};


	}
}
