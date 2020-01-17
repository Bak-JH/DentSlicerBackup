#pragma once
#include <QtQuick/private/qquickrectangle_p.h>
#include <QtQuick/private/qquickmousearea_p.h>
#include <QtQuick/private/qquickevents_p_p.h>
#include <QtQuick/private/qquicktext_p.h>


namespace Hix
{
	namespace QML
	{
		class ModalShell : public QQuickRectangle
		{
			Q_OBJECT
			Q_PROPERTY(QString modalmsg MEMBER _modalmsg NOTIFY modalmsgChanged)

		public:
			ModalShell(QQuickItem* parent = nullptr);

		public slots:
			void modalmsgChanged();

		protected:
			QString _modalmsg = "Are you sure?";
		};
		
	}
}
