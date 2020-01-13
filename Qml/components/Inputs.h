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
			Q_PROPERTY(QString labelText MEMBER _labelText NOTIFY labelTextChanged)
			Q_PROPERTY(int value MEMBER _value NOTIFY valueChanged)

		public:
			InputBox(QQuickItem* parent = nullptr);
			virtual ~InputBox();

		signals:
			void valueChanged();
			void labelTextChanged();

		private:
			QString _labelText = "Inputbox";
			int _value = 0;
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
