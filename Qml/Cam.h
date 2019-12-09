#pragma once
#include <QtQuick/private/qquickrectangle_p.h>
#include <QtQuick/private/qquickmousearea_p.h>
#include <QtQuick/private/qquickevents_p_p.h>
#include <QtQuick/private/qquicktext_p.h>


namespace Hix
{
	namespace QML
	{
		class CamButtonShell : public QQuickRectangle
		{
			Q_OBJECT

		public:
			CamButtonShell(QQuickItem* parent = nullptr);

		public slots:
			void onClick();

		private:
			QQuickMouseArea* _mouseArea;

		};

		class CamPopupShell : public QQuickRectangle
		{
			Q_OBJECT

		public:
			CamPopupShell(QQuickItem* parent = nullptr);
		};

		class CamBoxIcon : public QQuickRectangle
		{
			Q_OBJECT

		public:
			CamBoxIcon(QQuickItem* parent = nullptr);

		public slots:
			void onClick();

		private:
			QQuickMouseArea* _mouseArea;
		};
	}
}
