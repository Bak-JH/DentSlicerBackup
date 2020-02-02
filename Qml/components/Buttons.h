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
					Q_PROPERTY(bool isLeft MEMBER _isLeft)
			public:
				ToggleSwitch(QQuickItem* parent = nullptr);
				virtual ~ToggleSwitch();
				void initialize(QVariant leftVal, QVariant rightVal);
				//void setIsLeft(bool isLeft);
				QVariant value()const;
			signals:
				void checkedChanged();

			private slots:
				void onClicked()override;

			private:
				QVariant _leftVal;
				QVariant _rightVal;
				bool _isLeft = true;
			};
		}

	}
}