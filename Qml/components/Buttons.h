#pragma once
#include <QQuickItem>
#include <qvariant.h>
#include "InputControl.h"

namespace Hix
{
	namespace QML
	{
		namespace Controls
		{
			/// Button base class ///
			class Button : public QQuickItem, public InputControl
			{
				Q_OBJECT
			public:
				Button(QQuickItem* parent = nullptr);
				virtual ~Button();

			signals:
				void clicked();
				//void entered();
				//void exited();
				//void positionChanged(QQuickMouseEvent* mouse);

			protected slots:
				virtual void onClicked();
				//virtual void onEntered();
				//virtual void onExited();
				//virtual void onPositionChanged(QQuickMouseEvent* mouse);

			protected:
			};
			/// Toggle Switch ///
			class ToggleSwitch : public QQuickItem, public InputControl
			{
				Q_OBJECT
				Q_PROPERTY(bool isChecked MEMBER _isChecked NOTIFY checkedChanged)
			
			public:
				ToggleSwitch(QQuickItem* parent = nullptr);
				virtual ~ToggleSwitch();
				bool isChecked()const;
				void setChecked(bool isChecked);
			signals:
				void checkedChanged();
				void checked();
				void unchecked();

				void entered();
				void exited();

			protected slots:
				virtual void onCheckedChanged();
				virtual void onEntered();
				virtual void onExited();
				//virtual void onClicked();

			private:
				bool _isChecked = false; //isRight toggleImgSwitch
			};
		}

	}
}