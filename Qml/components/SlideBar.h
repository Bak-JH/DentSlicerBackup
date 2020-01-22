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
			Q_PROPERTY(double value MEMBER _value NOTIFY valueChanged)

		public:
			SlideBarShell(QQuickItem* parent = nullptr);
			virtual ~SlideBarShell();
			void setMin(double pMin);
			void setMax(double pMax);
			double getMin()const;
			double getMax()const;
			double getValue()const;

		signals:
			void minimumChanged();
			void maximumChanged();
			void valueChanged();

		private:
			double _min = 0, _max = 100, _value = 0;
		};

		class RangeSlideBarShell : public QQuickItem
		{
			Q_OBJECT
			Q_PROPERTY(double min MEMBER _min NOTIFY minimumChanged)
			Q_PROPERTY(double max MEMBER _max NOTIFY maximumChanged)
			Q_PROPERTY(double lowerValue MEMBER _lowerValue NOTIFY lowerValueChanged)
			Q_PROPERTY(double upperValue MEMBER _upperValue NOTIFY upperValueChanged)

		public:
			RangeSlideBarShell(QQuickItem* parent = nullptr);
			virtual ~RangeSlideBarShell();
			void setMin(double pMin);
			void setMax(double pMax);
			double getMin()const;
			double getMax()const;
			double lowerValue()const;
			double upperValue()const;


		signals:
			void minimumChanged();
			void maximumChanged();
			void lowerValueChanged();
			void upperValueChanged();

		private:
			double _min = 0, _max = 100, _lowerValue, _upperValue;
		};
	}
}
