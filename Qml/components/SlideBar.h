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
		class SlideBarShell : public QQuickRectangle
		{
			Q_OBJECT
			Q_PROPERTY(QQuickSlider* slideRect MEMBER _slideRect)

		public:
			SlideBarShell(QQuickItem* parent = nullptr);
			QQuickSlider* _slideRect;
		};
	}
}
