#pragma once
#include <qquickitem.h>
#include <qvariant.h>
#include <qqmlcomponent.h>

#include <string>
#include <functional>
#include <optional>
namespace Hix
{
	namespace QML
	{

		struct MenuButtonArg
		{
			std::string featureName;
			std::string defaultButtonImgPath;
			std::string activeButtonImgPath;
			std::function<void()>  functor;
		};
		/// Button base class ///
		class FeatureMenu : public QQuickItem
		{
			Q_OBJECT
		public:
			FeatureMenu(QQuickItem* parent = nullptr);
			virtual ~FeatureMenu();
			void addButton(const MenuButtonArg& args);
			void addDivider();
		protected:
			void componentComplete() override;
			

		private:
			std::optional<QQmlComponent> _component;
			QQuickItem* _featureItems;
		};
	}
}