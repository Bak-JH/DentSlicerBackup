#pragma once
#include <QtQuick/private/qquickrectangle_p.h>
#include <QtQuick/private/qquickmousearea_p.h>
#include <QtQuick/private/qquickevents_p_p.h>
#include <QtQuick/private/qquicktext_p.h>

namespace Hix
{
	namespace QML
	{
		class ViewModeShell : public QQuickItem
		{
			Q_OBJECT
			Q_PROPERTY(ViewType viewtype MEMBER _viewtype NOTIFY viewtypeChanged)
			Q_ENUMS(ViewType)

		public:
			ViewModeShell(QQuickItem* parent = nullptr);
			virtual ~ViewModeShell();

			enum ViewType { ObjectView, LayerView };

			Q_INVOKABLE void setViewType(ViewType type);
			ViewType getViewType()const;

		signals:
			void viewtypeChanged();

		protected:
			ViewType _viewtype = ViewType::ObjectView;

		};
		
	}
}
