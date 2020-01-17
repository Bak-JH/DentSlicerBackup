#pragma once
#include <QtQuick/private/qquickrectangle_p.h>
#include <QtQuick/private/qquickmousearea_p.h>
#include <QtQuick/private/qquickevents_p_p.h>
#include <QtQuick/private/qquicktext_p.h>


namespace Hix
{
	namespace QML
	{
		class ToastShell : public QQuickItem
		{
			Q_OBJECT
			Q_PROPERTY(QString toastmsg MEMBER _toastmsg NOTIFY toastmsgChanged)

			enum MesssageType { Done, Warning, Error };

		public:	
			ToastShell(QQuickItem* parent = nullptr);

		signals:
			void toastmsgChanged();

		//private:
			//QQuickMouseArea* _mouseArea;

		protected:
			QString _toastmsg = "Support generation completed";
		};
		
	}
}
