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


		class FeatureMenu : public QQuickItem
		{
			Q_OBJECT
		public:
			FeatureMenu(QQuickItem* parent = nullptr);
			virtual ~FeatureMenu();
			QQuickItem* featureItems();

		protected:
			void componentComplete() override;
			

		private:
			QQuickItem* _featureItems;
		};
	}
}