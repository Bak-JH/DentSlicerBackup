#pragma once
#include <QtQuick/private/qquickrectangle_p.h>
#include <QtQuick/private/qquickmousearea_p.h>
#include <QtQuick/private/qquickevents_p_p.h>
#include <QtQuick/private/qquicktext_p.h>


namespace Hix
{
	namespace QML
	{
		class ToastShell : public QQuickRectangle
		{
			Q_OBJECT
			Q_PROPERTY(QString toastmsg MEMBER _toastmsg NOTIFY toastmsgChanged)

		public:
			ToastShell(QQuickItem* parent = nullptr);

		public slots:
			void toastmsgChanged();

		//private:
			//QQuickMouseArea* _mouseArea;

		protected:
			QString _toastmsg = "Support generation completed";
		};
		
	}
}
