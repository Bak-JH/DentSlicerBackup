#pragma once
#include "../common/Singleton.h"
#include <QQmlApplicationEngine>
#include "PartManager.h"
#include "ModalDialogManager.h"

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
			ApplicationManager() = default;
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
			Qt3DCore::QEntity* getSceneRoot()const;

			PartManager& partManager();
			ModalDialogManager& modalDialogManager();
			void stateChanged();

		private:
			QQmlApplicationEngine _engine;
			PartManager _partManager;
			ModalDialogManager _modalManager;

			//mainview
			Qt3DCore::QEntity* _sceneRoot;
			//root for all mesh entities including 3D uis TODO:TEMP
			Qt3DCore::QEntity* _entityRoot;
			QQuickItem* _scene3D;

			//root container of qquickitems in the MAIN application
			QQuickItem* _uiRoot;
			//root of entire window application ie) including login window
			QQuickItem* _windowRoot;
			Hix::QML::PrintInfo* _printInfo;
		};


	}
}
