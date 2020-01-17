#pragma once
#include <QtQuick/private/qquickrectangle_p.h>
#include <QtQuick/private/qquickmousearea_p.h>
#include <QtQuick/private/qquickevents_p_p.h>
#include <QtQuick/private/qquicktext_p.h>
#include <QtQuickTemplates2/private/qquickslider_p.h>

namespace Hix
{
	namespace QML
	{
		class SlideBarShell : public QQuickItem
		{
			Q_OBJECT
			Q_PROPERTY(double min MEMBER _min NOTIFY minimumChanged)
			Q_PROPERTY(double max MEMBER _max NOTIFY maximumChanged)

		public:
			SlideBarShell(QQuickItem* parent = nullptr);
			virtual ~SlideBarShell();

		signals:
			void minimumChanged();
			void maximumChanged();

		private:
			double _min = 0, _max = 100;
		};

		class RangeSlideBarShell : public QQuickItem
		{
			Q_OBJECT
				Q_PROPERTY(double min MEMBER _min NOTIFY minimumChanged)
				Q_PROPERTY(double max MEMBER _max NOTIFY maximumChanged)

		public:
			RangeSlideBarShell(QQuickItem* parent = nullptr);
			virtual ~RangeSlideBarShell();

		signals:
			void minimumChanged();
			void maximumChanged();

		private:
			double _min = 0, _max = 100;
		};
	}
}
