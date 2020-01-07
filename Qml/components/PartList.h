#pragma once
#include <QtQuick/private/qquickrectangle_p.h>
#include <QtQuick/private/qquickmousearea_p.h>
#include <QtQuick/private/qquickevents_p_p.h>
#include <QtQuick/private/qquicktext_p.h>

namespace Hix
{
	namespace QML
	{
		class PartListContent : public QQuickRectangle
		{
			Q_OBJECT
			Q_PROPERTY(QString modelname MEMBER _modelname)
		public:
			PartListContent(QQuickItem* parent = nullptr);

		public slots:
			void onClick();

		private:
			QQuickMouseArea* _mouseArea;

		protected:
			QString _modelname = "testmodel.stl";
		};

		class PartDeleteButton : public QQuickRectangle
		{
			Q_OBJECT

		public:
			PartDeleteButton(QQuickItem* parent = nullptr);

		public slots:
			void onClick();

		private:
			QQuickMouseArea* _mouseArea;

		};
	}
}