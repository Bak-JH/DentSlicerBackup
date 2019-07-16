#pragma once
#include "RotateXYZWidget.h"
#include "MoveXYZWidget.h"
//manages rotate/move widgets
class QmlManager;
namespace Hix
{
	namespace Input
	{
		class RayCastController;
	}
}

namespace Qt3DCore
{
	class QEntity;
}
namespace Hix
{
	namespace UI
	{
		enum WidgetMode
		{
			None = 0,
			Move,
			Rotate
		};
		class Widget3DManager
		{
		public:
			void initialize(Qt3DCore::QEntity* qParent, Input::RayCastController* controller);
			void setWidgetMode(WidgetMode mode);
			void updatePosition();
		private:
			WidgetMode _currMode = WidgetMode::None;
			RotateXYZWidget _rotateWidget;
			MoveXYZWidget _moveWidget;
			Input::RayCastController* _controller;
		};
	}
}
