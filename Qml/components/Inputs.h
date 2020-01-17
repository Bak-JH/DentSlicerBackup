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
		class InputSpinBox : public QQuickItem
		{
			Q_OBJECT
			Q_PROPERTY(QString labelText MEMBER _labelText NOTIFY labelTextChanged)
			Q_PROPERTY(int value MEMBER _value NOTIFY valueChanged)

		public:
			InputSpinBox(QQuickItem* parent = nullptr);
			virtual ~InputSpinBox();

		signals:
			void valueChanged();
			void labelTextChanged();

		private:
			QString _labelText = "Inputbox";
			int _value = 0;
		};

		class TextInputBox : public QQuickItem
		{
			Q_OBJECT
			Q_PROPERTY(QString labelText MEMBER _labelText NOTIFY labelTextChanged)
			Q_PROPERTY(QString inputText MEMBER _text NOTIFY inputTextChanged)

		public:
			TextInputBox(QQuickItem* parent = nullptr);
			virtual ~TextInputBox();

		signals:
			void inputTextChanged();
			void labelTextChanged();

		private:
			QString _labelText = "Inputbox";
			QString _text;
		};

		class DropdownBox : public QQuickRectangle
		{
			Q_OBJECT
			Q_PROPERTY(QString labelText MEMBER _labelText NOTIFY labelTextChanged)
			Q_PROPERTY(QStringList dropList MEMBER _dropList)

		public:
			DropdownBox(QQuickItem* parent = nullptr);
			virtual ~DropdownBox();

		signals:
			void labelTextChanged();

		protected:
			QString _labelText = "Dropdown";
			QStringList _dropList;
		};
	}
}
