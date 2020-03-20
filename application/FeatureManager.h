#pragma once
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <qvector3d.h>
#include "../feature/FeatureHistoryManager.h"
#include "../feature/interfaces/Mode.h"
class GLModel;
class QQuickItem;
namespace Qt3DCore
{
	class QEntity;
	class QTransform;
}
namespace Qt3DRender
{
	class QCamera;
	class QCameraLens;
}
namespace Hix
{
	namespace QML
	{
		class PartList;
		class PrintInfo;
		class FeatureMenu;
		namespace Controls
		{
			class Button;
		}
	}
	namespace Features
	{
		class Mode;
		class ListModel;
	}
	namespace Application
	{
		class ApplicationManager;
		class FeatureManager
		{
		public:

			template<typename FeatureType>
			bool isActive()const
			{
				if (_currentMode.get() != nullptr)
				{
					return dynamic_cast<const FeatureType*>(_currentMode.get()) != nullptr;
				}
				return false;
			}
			bool isFeatureActive();

			FeatureManager();
			FeatureManager(const FeatureManager& other) = delete;
			FeatureManager(FeatureManager&& other) = delete;
			FeatureManager& operator=(FeatureManager other) = delete;
			FeatureManager& operator=(FeatureManager&& other) = delete;
			QQuickItem* featureArea();
			QQuickItem* slideArea();
			Hix::Features::Mode* currentMode()const;
			void setMode(Hix::Features::Mode*);
			Hix::Features::FeatureHistoryManager& featureHistoryManager();
			bool allowModelSelection()const;
			void setViewModeSwitch(bool isSet);
		private:
			Hix::QML::Controls::ToggleSwitch* _viewSwitch;
			Hix::QML::Controls::Button* _settingButton;
			Hix::QML::Controls::Button* _undoButton;
			Hix::QML::Controls::Button* _redoButton;
			Hix::QML::FeatureMenu* _menu;
			QQuickItem* _featureArea;
			QQuickItem* _slideArea;
			std::unique_ptr<Hix::Features::Mode> _currentMode;
			Hix::Features::FeatureHistoryManager _featureHistoryManager;


			friend class FeatureManagerLoader;
		};

		class FeatureManagerLoader
		{
		private:
			static void init(FeatureManager& manager, QObject* root);
			friend class Hix::Application::ApplicationManager;
		};





	}
}
