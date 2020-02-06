#pragma once
#include <QtQuick/private/qquicktext_p.h>
#include <QtQuick/private/qquickimage_p.h>
#include <QtQuick/private/qquickrectangle_p.h>
#include <QtQuick/private/qquickmousearea_p.h>
#include <QtQuick/private/qquickevents_p_p.h>
#include <qvariant.h>
#include "InputControl.h"

namespace Hix
{
	namespace QML
	{
		namespace Controls
		{
			/// Button base class ///
			class Button : public QQuickRectangle, public InputControl
			{
				Q_OBJECT
			public:
				Button(QQuickItem* parent = nullptr);
				virtual ~Button();

			signals:
				void clicked();
				void entered();
				void exited();

			protected slots:
				virtual void onClicked();
				virtual void onEntered();
				virtual void onExited();

			protected:
				QQuickMouseArea* _mouseArea;
			};
			/// Toggle Switch ///
			class ToggleSwitch : public Button
			{
				Q_OBJECT
				Q_PROPERTY(bool isChecked MEMBER _isChecked NOTIFY checked)
			
			public:
				ToggleSwitch(QQuickItem* parent = nullptr);
				virtual ~ToggleSwitch();
				bool isChecked()const;
				void setChecked(bool isChecked);
			signals:
				void checked();
				void unchecked();

			private slots:
				void onClicked()override;

			private:
				bool _isChecked = false;
			};
		}

	}
}