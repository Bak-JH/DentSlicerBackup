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

		public slots:
			virtual void onClick() = 0;
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

		public slots:
			void onClick()override;
		};

		/// Round Button ///
		class RoundButton : public Button
		{
			Q_OBJECT
		public:
			RoundButton(QQuickItem* parent = nullptr);
			virtual ~RoundButton();

		signals:
			void clicked();
			void entered();
			void exited();

		public slots:
			void onClick()override;
			void onEntered()override;
			void onExited()override;
			void fNameChanged();
		};

		/// Menu Button ///
		class MenuButton : public Button
		{
			Q_OBJECT
		public:
			MenuButton(QQuickItem* parent = nullptr);
			virtual ~MenuButton();

		signals:
			void clicked();
			void entered();
			void exited();

		public slots:
			void onClick()override;
			void onEntered()override;
			void onExited()override;
		};

		/// Toggle Switch ///
		class ToggleSwitch : public Button
		{
			Q_OBJECT
			Q_PROPERTY(bool isChecked MEMBER _isChecked)

		public:
			ToggleSwitch(QQuickItem* parent = nullptr);
			virtual ~ToggleSwitch();

		public slots:
			void onClick()override;

		private:
			bool _isChecked = false;
		};

		/// Image Toggle Switch ///
		class ImageToggleSwitch : public Button
		{
			Q_OBJECT
			Q_PROPERTY(bool isChecked MEMBER _isChecked)

		public:
			ImageToggleSwitch(QQuickItem* parent = nullptr);
			virtual ~ImageToggleSwitch();

		public slots:
			void onClick()override;

		private:
			bool _isChecked = false;
		};
	}
}