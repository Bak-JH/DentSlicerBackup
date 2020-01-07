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
		/// CloseButton ///
		class CloseButton : public QQuickRectangle
		{
			Q_OBJECT
				Q_PROPERTY(QQuickImage* image MEMBER _image)

		public:
			CloseButton(QQuickItem* parent = nullptr);

		public slots:
			void onClick();

		private:
			QQuickMouseArea* _mouseArea;
			QQuickImage* _image;
		};

		/// Round Button ///
		class RoundButton : public QQuickRectangle
		{
			Q_OBJECT
			Q_PROPERTY(QQuickText* labelText MEMBER _labelText)

		public:
			RoundButton(QQuickItem* parent = nullptr);

		signals:
			void clicked();
			void entered();
			void exited();

		public slots:
			void onClick();
			void onEntered();
			void onExited();
			void fNameChanged();
			//void onEnable();
			//void onDisable();

		protected:
			QQuickText* _labelText;
			QString _fName = "Name";
			QQuickMouseArea* _mouseArea;
		};
	}
}