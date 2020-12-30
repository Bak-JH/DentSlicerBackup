#pragma once
#include "feature/interfaces/Mode.h"
#include "widget/Widget3D.h"
#include "input/raycastcontroller.h"

namespace Hix
{
	namespace Features
	{
		void UpdateWidgetModePos();
		class WidgetMode : virtual public Mode
		{
		public:
			WidgetMode();
			virtual ~WidgetMode();
			virtual void updatePosition();
			virtual void featureStarted() = 0;
			virtual void featureEnded() = 0;
			virtual QVector3D getWidgetPosition() = 0;
		protected:
			void updateTransform(Qt3DCore::QTransform& transform);
			UI::Widget3D _widget;
			Input::RayCastController* _controller;
		};
	}
}