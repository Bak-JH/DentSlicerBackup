#pragma once
#include <string>
#include <QQuickItem>
#include "../../common/magic_enum.hpp"
#include "InputControl.h"
namespace Hix
{
	namespace QML
	{
		namespace Controls
		{
			//warning! Now siginificant figure aware, will clean value if it can
			class InputSpinBox : public QQuickItem, public InputControl
			{
				Q_OBJECT
					Q_PROPERTY(double value MEMBER _value NOTIFY valueChanged)
					Q_PROPERTY(double min MEMBER _min NOTIFY rangeChanged)
					Q_PROPERTY(double max MEMBER _max NOTIFY rangeChanged)
					Q_PROPERTY(double increment MEMBER _increment NOTIFY incrementChanged)

			public:
				InputSpinBox(QQuickItem* parent = nullptr);
				virtual ~InputSpinBox();
				double getValue()const;
				void setValue(double value);
				void setRange(double min, double max);
				void setIncrement(double inc);
				void updateValue();
				//Q_INVOKABLE QVariantMap parseStringSigFig(QString str)const;
				///// <summary>
				///// Pretty damn accurate sig fig guessing from given double
				///// </summary>
				///// <param name="dbl"> "dirty" double number</param>
				///// <returns>json equivalnet of value: doubleNumber, digits: validDec </returns>
				//Q_INVOKABLE QVariantMap guessDoubleSigFig(QString str)const;


			signals:
				void valueChanged();
				void rangeChanged();
				void incrementChanged();


			private:
				double _value = 0;
				double _min = std::numeric_limits<double>::min(), _max = std::numeric_limits<double>::max();
				double _increment = 1.0;
			};

			class TextInputBox : public QQuickItem, public InputControl
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

			class DropdownBox : public QQuickItem, public InputControl
			{
				Q_OBJECT
				Q_PROPERTY(QStringList dropList MEMBER _dropList NOTIFY listChanged)
				Q_PROPERTY(int index MEMBER _index NOTIFY indexChanged)

			public:
				DropdownBox(QQuickItem* parent = nullptr);
				virtual ~DropdownBox();
				int getIndex()const;
				size_t size()const;
				QString getSelectedItem()const;
				void setIndex(int index);
				void setList(QStringList list);
				void appendList(QString item);
				const QStringList& list()const;


				
				//for use with enum
				template<typename EnumType>
				void setEnums(EnumType initialValue)
				{
					auto enumStrs = magic_enum::enum_names<EnumType>();
					auto iniIndex = magic_enum::enum_index(initialValue);

					QStringList list;
					for (auto& str : enumStrs)
					{

						list.push_back(QString::fromStdString(std::string(str)));
					}
					setList(list);
					setIndex(iniIndex.value());
				}
				template<typename EnumType>
				void getSelected(EnumType& out)
				{
					auto& str = _dropList[getIndex()];
					out = magic_enum::enum_cast<EnumType>(str.toStdString()).value();
				}

			signals:
				void indexChanged();
				void listChanged();

			protected:
				QStringList _dropList;
				int _index = 0;
			};
		}

	}
}
