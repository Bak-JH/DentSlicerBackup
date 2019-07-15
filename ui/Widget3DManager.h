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
		class Widget3DManager
		{
		public:
			enum WidgetMode
			{
				None = 0,
				Move,
				Rotate
			};
			Widget3DManager(Qt3DCore::QEntity* qParent, Input::RayCastController* controller);
			void setWidgetMode(WidgetMode mode);
		private:
			WidgetMode _currMode;
			RotateXYZWidget _rotateWidget;
			MoveXYZWidget _moveWidget;

		};
	}
}
