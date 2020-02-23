#pragma once
#include <qqmlcomponent.h>
#include <string>
#include "../application/ApplicationManager.h"

class QQuickItem;
namespace Hix
{
	namespace QML
	{
		class FeatureMenu;
		namespace Controls
		{
			class Button;
			class ToggleSwitch;
		}
	}
	namespace Features
	{
		template<typename ModeType>
		std::function<void()> openFeatureModeFunctor(Hix::QML::Controls::ToggleSwitch* button)
		{
			static_assert(std::is_base_of<Hix::Features::Mode, ModeType>{});
			std::function<void()> functor = [button]() {
				if (!Hix::Application::ApplicationManager::getInstance().featureManager().isFeatureActive())
				{
					try
					{
						ModeType* newMode = new ModeType();
						newMode->addButton(button);
						Hix::Application::ApplicationManager::getInstance().featureManager().setMode(newMode);
					}
					catch (...)
					{
						qDebug() << "mode creation failed";
						Hix::Application::ApplicationManager::getInstance().featureManager().setMode(nullptr);
					}

				}
			};
			return functor;
		}

		class FeaturesLoader
		{
		public:
			FeaturesLoader(QQmlEngine* e, Hix::QML::FeatureMenu* menu);
			void loadFeatureButtons();
			void addDivider();
			QQmlComponent& parsedComp();
			Hix::QML::FeatureMenu& menu();



		private:
			QQmlComponent _component;
			Hix::QML::FeatureMenu* _menu;
		};
	}
}


