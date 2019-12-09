  #pragma once
#include <QtQuick/private/qquickrectangle_p.h>
#include <QtQuick/private/qquickmousearea_p.h>
#include <QtQuick/private/qquickevents_p_p.h>
#include <QtQuick/private/qquicktext_p.h>
#include <QtQuickTemplates2/private/qquickspinbox_p.h>
#include <QtQuickTemplates2/private/qquickcombobox_p.h>
#include <QSpinBox>
//#include <QComboBox>
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
			QString _popupHeight = "360"; //"284";
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
			QString _propName = "Property";
		};

		class DropdownBox : public QQuickRectangle
		{
			Q_OBJECT
			Q_PROPERTY(QString dropName MEMBER _dropName NOTIFY dropNameChanged)
			Q_PROPERTY(QQuickComboBox* dropRect MEMBER _dropRect)
			Q_PROPERTY(QStringList dropList MEMBER _dropList)

		public:
			DropdownBox(QQuickItem* parent = nullptr);
			QQuickComboBox* _dropRect;

		public slots:
			void dropNameChanged();

		protected:
		
			QString _dropName = "Dropdown";
			QStringList _dropList = { "Banana", "Apple", "Coconut", "Almond" };
		};

		class FreeCutButton : public QQuickRectangle
		{
			Q_OBJECT
			Q_PROPERTY(QColor btncolor MEMBER _btncolor NOTIFY btnColorChanged)

		public:
			FreeCutButton(QQuickItem* parent = nullptr);

		signals:
			void btnColorChanged(const QColor& changedColor);

		public slots:
			void onClick();

		private:
			QQuickMouseArea* _mouseArea;
			QColor _btncolor;
		};

		class FlatCutButton : public QQuickRectangle
		{
			Q_OBJECT

		public:
			FlatCutButton(QQuickItem* parent = nullptr);

		public slots:
			void onClick();

		private:
			QQuickMouseArea* _mouseArea;

		};

		class LabelTextBox : public QQuickRectangle
		{
			Q_OBJECT

		};

		class RoundButton : public QQuickRectangle
		{
			Q_OBJECT
			Q_PROPERTY(QString btntext MEMBER _btntext NOTIFY btntextChanged)
			Q_PROPERTY(QColor btncolor MEMBER _btncolor NOTIFY btnColorChanged)
			//Q_PROPERTY(QString btnwidth MEMBER _btnwidth NOTIFTY btnwidthChanged)

		public:
			RoundButton(QQuickItem* parent = nullptr);

		signals:
			void btnColorChanged(const QColor& changedColor);

		public slots:
			void btntextChanged();
			//void btnwidthChanged();
			void onClick();
			void onEntered();
			void onExited();

		private:
			QQuickMouseArea* _mouseArea;
			QString _btntext = "Apply";
			QColor _btncolor;
			//QString _btnwidth = "224";
		};
	}
}