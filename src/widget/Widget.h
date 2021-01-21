#pragma once
#include <QObject>
#include <QApplication>
#include <Qt3DCore>
#include <Qt3DRender>
#include <Qt3DExtras>
#include <Qt3DInput>
#include "../input/HitTestAble.h"
#include "../input/Draggable.h"
#include "../input/Hoverable.h"
#include "Highlightable.h"

namespace Hix
{
	namespace UI
	{
		class Widget3D;
		class Widget: 
			public Qt3DCore::QEntity, public Hix::Input::HitTestAble, 
			public Hix::Input::Draggable, public Hix::Input::Hoverable, public Hix::UI::Highlightable
		{
		public:
			Widget(const QVector3D& axis, Qt3DCore::QEntity* parent);
			virtual ~Widget();
		protected:
			void initHitTest()override;
			QVector3D _axis;
			Widget3D* _parent;
		};
	}
}

