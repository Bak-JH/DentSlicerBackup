#pragma once
#include <QtQuick/private/qquickrectangle_p.h>
#include <QtQuick/private/qquicktext_p.h>
#include <QtQuick/private/qquicktextinput_p.h>
#include <QtQuickTemplates2/private/qquickspinbox_p.h>
#include <QtQuickTemplates2/private/qquickcombobox_p.h>
#include <string>

namespace Hix
{
	namespace QML
	{
		class InputSpinBox : public QQuickItem
		{
			Q_OBJECT
			Q_PROPERTY(int value MEMBER _value NOTIFY valueChanged)

		public:
			InputSpinBox(QQuickItem* parent = nullptr);
			virtual ~InputSpinBox();
			int getValue()const;
			void setValue(int value);

		signals:
			void valueChanged();

		private:
			int _value = 0;
		};

		class TextInputBox : public QQuickItem
		{
			Q_OBJECT
			Q_PROPERTY(QString inputText MEMBER _text NOTIFY inputTextChanged)

		public:
			TextInputBox(QQuickItem* parent = nullptr);
			virtual ~TextInputBox();
			std::string getInputText()const;
			void setInputText(std::string text);

		signals:
			void inputTextChanged();

		private:
			QString _text;
		};

		class DropdownBox : public QQuickRectangle
		{
			Q_OBJECT
			Q_PROPERTY(QStringList dropList MEMBER _dropList)
			Q_PROPERTY(int index MEMBER _index NOTIFY indexChanged)

		public:
			DropdownBox(QQuickItem* parent = nullptr);
			virtual ~DropdownBox();

		signals:
			void indexChanged();

		protected:
			QStringList _dropList;
			int _index;
		};
	}
}
