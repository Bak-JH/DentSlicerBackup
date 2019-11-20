#pragma once
#include <QtQuick/private/qquickrectangle_p.h>
#include <QtQuick/private/qquickmousearea_p.h>
#include <QtQuick/private/qquickevents_p_p.h>
#include <QtQuick/private/qquicktext_p.h>
#include <QtQuickTemplates2/private/qquickspinbox_p.h>
#include <QSpinBox>
#include <QWidget>


namespace Hix
{
	namespace QML
	{
		
		class CloseButton : public QQuickRectangle
		{
			Q_OBJECT

		public:
			CloseButton(QQuickItem* parent = nullptr);

		public slots:
			void onClick();

		private:
			QQuickMouseArea* _mouseArea;
		};
		

		class LeftPopupShell : public QQuickRectangle
		{
			Q_OBJECT
			Q_PROPERTY(QString title MEMBER _title NOTIFY titleChanged)
			Q_PROPERTY(QString popupHeight MEMBER _popupHeight NOTIFY popupHeightChanged)

		public:
			LeftPopupShell(QQuickItem* parent = nullptr);

		public slots:
			void titleChanged();
			void popupHeightChanged();

		protected:
			QString _title = "Name";
			QString _popupHeight = "284";
		};


		class InputBox : public QQuickRectangle
		{
			Q_OBJECT
			Q_PROPERTY(QString propName MEMBER _propName NOTIFY propNameChanged)
			Q_PROPERTY(QQuickSpinBox* inputRect MEMBER _inputRect)

		public:
			InputBox(QQuickItem* parent = nullptr);
			QQuickSpinBox* _inputRect;

		public slots:
			void propNameChanged();

		protected:
			QString _propName = "property";
		};


		class RoundButton : public QQuickRectangle
		{
			Q_OBJECT
			Q_PROPERTY(QString btntext MEMBER _btntext)

		public:
			RoundButton(QQuickItem* parent = nullptr);

		public slots:
			void textChanged();
			void onClick();

		private:
			QQuickMouseArea* _mouseArea;
			QString _btntext = "Apply";
		};
	}
}