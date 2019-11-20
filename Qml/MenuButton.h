#pragma once
#include <QtQuick/private/qquickrectangle_p.h>
#include <QtQuick/private/qquickmousearea_p.h>
#include <QtQuick/private/qquickevents_p_p.h>
#include <QtQuick/private/qquicktext_p.h>

namespace Hix
{
	namespace QML
	{
		class MenuBtnShell : public QQuickRectangle
		{
			Q_OBJECT
			Q_PROPERTY(QString fname MEMBER _fname)
			//Q_PROPERTY(QImage fimg MEMBER _fimg)
		public:
			MenuBtnShell(QQuickItem* parent = nullptr);

		public slots:
			void onClick();
		
		private:
			QQuickMouseArea* _mouseArea;

		protected:
			QString _fname = "name";
			//QImage _fimg = QImage("qrc:/Resource/vs_icon.png");
		};
	}
}