#pragma once
#include <QtQuick/private/qquickrectangle_p.h>
#include <QtQuick/private/qquicktext_p.h>
#include <QtQuick/private/qquicktextinput_p.h>
#include <QtQuickTemplates2/private/qquickspinbox_p.h>
#include <QtQuickTemplates2/private/qquickcombobox_p.h>

namespace Hix
{
	namespace QML
	{
		class InputSpinBox;
		class InputBox : public QQuickItem
		{
			Q_OBJECT
				//Q_PROPERTY(QString propName MEMBER _propName NOTIFY propNameChanged)
			Q_PROPERTY(QQuickText* label MEMBER _label)
			Q_PROPERTY(InputSpinBox* spinbox READ spinbox CONSTANT FINAL)

		public:
			InputBox(QQuickItem* parent = nullptr);
			InputSpinBox* spinbox()const;

		signals:
			void propertyChanged();

		private:
			QQuickText* _label;
			InputSpinBox* _spinbox;
		};

		class InputSpinBox : public QQuickSpinBox
		{
			Q_OBJECT
		public:
			InputSpinBox(QQuickItem* parent = nullptr);
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
	}
}
