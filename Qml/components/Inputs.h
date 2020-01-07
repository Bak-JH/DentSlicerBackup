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
			Q_PROPERTY(QQuickText* label MEMBER _label)
			Q_PROPERTY(int value MEMBER _value)

		public:
			InputBox(QQuickItem* parent = nullptr);
			Q_INVOKABLE void test() { qDebug() << _value; }
		signals:
			void propertyChanged();

		private:
			QQuickText* _label;
			int _value = 0;
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
