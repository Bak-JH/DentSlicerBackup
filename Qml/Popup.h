#pragma once
#include <QtQuick/private/qquickrectangle_p.h>
#include <QtQuick/private/qquickmousearea_p.h>
#include <QtQuick/private/qquickevents_p_p.h>
#include <QtQuick/private/qquicktext_p.h>

namespace Hix
{
	namespace QML
	{
		class XButton : public QQuickRectangle
		{
			Q_OBJECT
		public:
			XButton(QQuickItem* parent = nullptr);

		public slots:
			void onClick();

		private:
			QQuickMouseArea* _mouseArea;
		};

		class PopupShell : public QQuickRectangle
		{
			Q_OBJECT
			Q_PROPERTY(QString title MEMBER _title NOTIFY titleChanged)

		public:
			PopupShell(QQuickItem* parent = nullptr);

		public slots:
			void titleChanged();

		protected:
			QString _title = "Test";
		};
	}
}