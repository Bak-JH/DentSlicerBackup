#pragma once
#include <QtQuick/private/qquicktext_p.h>
#include <QtQuick/private/qquickimage_p.h>
#include <QtQuick/private/qquickrectangle_p.h>
#include <QtQuick/private/qquickmousearea_p.h>
#include <QtQuick/private/qquickevents_p_p.h>

namespace Hix
{
	namespace QML
	{
		/// Button base class ///
		class Button : public QQuickRectangle
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

		/// CloseButton ///
		class CloseButton : public Button
		{
			Q_OBJECT
		public:
			CloseButton(QQuickItem* parent = nullptr);
			virtual ~CloseButton();

		private slots:
			void onClicked()override;
		};

		/// Toggle Switch ///
		class ToggleSwitch : public Button
		{
			Q_OBJECT
			Q_PROPERTY(bool isChecked MEMBER _isChecked NOTIFY checkedChanged)

		public:
			ToggleSwitch(QQuickItem* parent = nullptr);
			virtual ~ToggleSwitch();
			bool isChecked()const;

		signals:
			void checkedChanged();

		private slots:
			void onClicked()override;

		private:
			bool _isChecked = false;
		};

		/// Image Toggle Switch ///
		class ImageToggleSwitch : public QQuickItem
		{
			Q_OBJECT
			Q_PROPERTY(bool isChecked MEMBER _isChecked)

		public:
			ImageToggleSwitch(QQuickItem* parent = nullptr);
			virtual ~ImageToggleSwitch();
			bool isChecked()const;
			Q_INVOKABLE void setChecked(bool isChecked);

		private:
			bool _isChecked = false;
		};
	}
}