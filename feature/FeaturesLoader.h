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
		std::function<void()> openFeatureModeFunctor(Hix::QML::Controls::ToggleSwitch* button = nullptr)
		{
			static_assert(std::is_base_of<Hix::Features::Mode, ModeType>{});
			std::function<void()> functor = [button]() {
				try
				{
					//we need to delete previous task manually, as button uncheck function can close newly opened feature mode.
					if (Hix::Application::ApplicationManager::getInstance().featureManager().isFeatureActive())
					{
						Hix::Application::ApplicationManager::getInstance().featureManager().setMode(nullptr);
					}
					ModeType* newMode = new ModeType();
					newMode->addButton(button);
					Hix::Application::ApplicationManager::getInstance().featureManager().setMode(newMode);
						
				}
				catch (const std::runtime_error& e)
				{
					qDebug() << e.what();
					Hix::Application::ApplicationManager::getInstance().featureManager().setMode(nullptr);
				}
				catch (...)
				{
					qDebug() << "mode creation failed";
					Hix::Application::ApplicationManager::getInstance().featureManager().setMode(nullptr);
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
			QQmlComponent _dividerComp;

			Hix::QML::FeatureMenu* _menu;
		};
	}
}


