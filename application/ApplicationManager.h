#pragma once
#include "../common/Singleton.h"
#include <QQmlApplicationEngine>
#include "PartManager.h"
class QQuickItem;
namespace Qt3DCore
{
	class QEntity;
}
namespace Hix
{
	namespace Application
	{
		class ApplicationManager: public Hix::Common::Singleton<ApplicationManager>
		{
		public:
			QQmlApplicationEngine& engine();
			void init();
			QQuickItem* getUIRoot()const;
		private:
			QQmlApplicationEngine _engine;
			PartManager _manager;
			Qt3DCore::QEntity* _sceneRoot;
			QQuickItem* _uiRoot;
		};


	}
}
