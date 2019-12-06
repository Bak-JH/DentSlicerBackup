#pragma once
#include "feature/interfaces/Mode.h"
#include "ui/Widget3D.h"
#include "input/raycastcontroller.h"

namespace Hix
{
	namespace Features
	{
		class WidgetMode : public Mode
		{
		public:
			WidgetMode(const std::unordered_set<GLModel*>& targetModels, Input::RayCastController* controller);
			virtual ~WidgetMode();
			void updatePosition();
			virtual void featureStarted() = 0;
			virtual void featureEnded() = 0;

		protected:
			std::unordered_set<GLModel*> _targetModels;
			UI::Widget3D _widget;
			Input::RayCastController* _controller;
		};
	}
}