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
			PartManager& partManager();
			ModalDialogManager& modalDialogManager();
			void stateChanged();

		private:
			QQmlApplicationEngine _engine;
			PartManager _partManager;
			ModalDialogManager _modalManager;
			Qt3DCore::QEntity* _sceneRoot;
			QQuickItem* _uiRoot;
			Hix::QML::PrintInfo* _printInfo;
		};


	}
}
