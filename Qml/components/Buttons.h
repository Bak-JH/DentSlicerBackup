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
		/// Button baseclass ///
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
			Q_PROPERTY(QQuickImage* image MEMBER _image)

		public:
			CloseButton(QQuickItem* parent = nullptr);
			virtual ~CloseButton();

		public slots:
			void onClick()override;

		private:
			QQuickImage* _image;
		};

		/// Round Button ///
		class RoundButton : public Button
		{
			Q_OBJECT
			Q_PROPERTY(QQuickText* labelText MEMBER _labelText)

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
			//void onEnable();
			//void onDisable();

		protected:
			QQuickText* _labelText;
			QString _fName = "Name";
		};

		class MenuButton : public Button
		{
			Q_OBJECT
			Q_PROPERTY(QQuickText* featureName MEMBER _name)
			Q_PROPERTY(QQuickImage* icon MEMBER _image)
		public:
			MenuButton(QQuickItem* parent = nullptr);
			virtual ~MenuButton();

		public slots:
			void onClick()override;
			void onEntered()override;
			void onExited()override;

		private:
			QQuickText* _name;
			QQuickImage* _image;
		};
	}
}