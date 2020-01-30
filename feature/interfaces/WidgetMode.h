#pragma once
#include "feature/interfaces/Mode.h"
#include "widget/Widget3D.h"
#include "input/raycastcontroller.h"

namespace Hix
{
	namespace Features
	{
		class WidgetMode : public Mode
		{
		public:
			WidgetMode();
			virtual ~WidgetMode();
			void updatePosition();
			virtual void featureStarted() = 0;
			virtual void featureEnded() = 0;
			const std::unordered_set<GLModel*>& models()const;
		protected:
			std::unordered_set<GLModel*> _targetModels;
			UI::Widget3D _widget;
			Input::RayCastController* _controller;
		};
	}
}