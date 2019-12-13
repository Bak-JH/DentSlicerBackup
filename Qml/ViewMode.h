#pragma once
#include <QtQuick/private/qquickrectangle_p.h>
#include <QtQuick/private/qquickmousearea_p.h>
#include <QtQuick/private/qquickevents_p_p.h>
#include <QtQuick/private/qquicktext_p.h>

namespace Hix
{
	namespace QML
	{
		class ViewModeShell : public QQuickRectangle
		{
			Q_OBJECT
			Q_PROPERTY(QString viewtype MEMBER _viewtype NOTIFY viewtypeChanged)

		public:
			ViewModeShell(QQuickItem* parent = nullptr);

		public slots:
			void viewtypeChanged();

		protected:
			QString _viewtype = "Object View";

		};
		
	}
}
