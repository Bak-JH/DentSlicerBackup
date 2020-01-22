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
			Q_PROPERTY(double value MEMBER _value NOTIFY valueChanged)
			Q_PROPERTY(double min MEMBER _min NOTIFY rangeChanged)
			Q_PROPERTY(double max MEMBER _max NOTIFY rangeChanged)

		public:
			InputSpinBox(QQuickItem* parent = nullptr);
			virtual ~InputSpinBox();
			double getValue()const;
			void setValue(double value);
			void setRange(double min, double max);

		signals:
			void valueChanged();
			void rangeChanged();

		private:
			double _value = 0;
			double _min, _max;
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

		class DropdownBox : public QQuickItem
		{
			Q_OBJECT
			Q_PROPERTY(QStringList dropList MEMBER _dropList NOTIFY listChanged)
			Q_PROPERTY(int index MEMBER _index NOTIFY indexChanged)

		public:
			DropdownBox(QQuickItem* parent = nullptr);
			virtual ~DropdownBox();
			int getIndex()const;
			void setIndex(int index);
			void setList(QStringList list);

		signals:
			void indexChanged();
			void listChanged();

		protected:
			QStringList _dropList;
			int _index;
		};
	}
}
