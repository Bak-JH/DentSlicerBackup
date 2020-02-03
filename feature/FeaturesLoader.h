#pragma once
#include <qqmlcomponent.h>
#include <string>
class QQuickItem;
namespace Hix
{
	namespace QML
	{
		class FeatureMenu;
		namespace Controls
		{
			class Button;
		}
	}
	namespace Features
	{
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


