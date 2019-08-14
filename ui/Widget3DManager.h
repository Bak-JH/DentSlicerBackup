#pragma once
#include "Widget3D.h"
#include "input/raycastcontroller.h"

//manages rotate/move/cube widgets
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
            void addCubeWidget(GLModel* parent, std::vector<QVector3D> overhangPoints);
			void setWidgetMode(WidgetMode mode);
			void updatePosition();
		private:
			WidgetMode _currMode = WidgetMode::None;
			Widget3D _rotateWidget;
			Widget3D _moveWidget;
			Widget3D _cubeWidget;
			Input::RayCastController* _controller;
		};
	}
}
