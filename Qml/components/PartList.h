#pragma once
#include <QtQuick/private/qquickrectangle_p.h>
#include <QtQuick/private/qquickmousearea_p.h>
#include <QtQuick/private/qquickevents_p_p.h>
#include <QtQuick/private/qquicktext_p.h>

namespace Hix
{
	namespace QML
	{
		class PartList : public QQuickItem
		{
			Q_OBJECT
		public:
			PartList(QQuickItem* parent = nullptr);
			virtual ~PartList();
			Q_INVOKABLE void appendModel(QString featureName);

		};

		class PartDeleteButton : public QQuickRectangle
		{
			Q_OBJECT
		public:
			PartDeleteButton(QQuickItem* parent = nullptr);
			virtual ~PartDeleteButton();
		public slots:
			void onClick();

		private:
			QQuickMouseArea* _mouseArea;

		};
	}
}