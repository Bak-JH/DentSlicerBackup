#pragma once
#include <QtQuick/private/qquickrectangle_p.h>
#include <QtQuick/private/qquickmousearea_p.h>
#include <QtQuick/private/qquickevents_p_p.h>
#include <QtQuick/private/qquicktext_p.h>


namespace Hix
{
	namespace QML
	{
		class ModalShell : public QQuickRectangle
		{
			Q_OBJECT
				Q_PROPERTY(QString modalmsg MEMBER _modalmsg NOTIFY modalmsgChanged)

		public:
			ModalShell(QQuickItem* parent = nullptr);

		public slots:
			void modalmsgChanged();

			//private:
				//QQuickMouseArea* _mouseArea;

		protected:
			QString _modalmsg = "Are you sure?";
		};
		class ModalCloseButton : public QQuickRectangle
		{
			Q_OBJECT

		public:
			ModalCloseButton(QQuickItem* parent = nullptr);

		public slots:
			void onClick();

		private:
			QQuickMouseArea* _mouseArea;
		};
		class ModalRoundButton : public QQuickRectangle
		{
			Q_OBJECT
			Q_PROPERTY(QString modalbtntext MEMBER _modalbtntext NOTIFY modalbtntextChanged)

		public:
			ModalRoundButton(QQuickItem* parent = nullptr);

		public slots:
			void onClick();
			void modalbtntextChanged();

		private:
			QQuickMouseArea* _mouseArea;

		protected:
			QString _modalbtntext = "Cancel";
		};
	}
}
