#pragma once
#include <QKeyboardHandler>
#include <QKeyboardDevice>

namespace Qt3DInput
{
	class QKeyEvent;
}
namespace Qt3DCore
{
	class QNode;
	class QEntity;
}
class QQuickItem;
namespace Hix
{
	namespace Application
	{
		class ApplicationManager;
	}
	namespace Input
	{
		class KeyboardController
		{
		public:
			KeyboardController();
			//outer-most key event handler
			void keyPressed(QKeyEvent* e);
			void keyReleased(QKeyEvent* e);
			virtual ~KeyboardController();
		private:

			bool _shiftPressed = false;
			bool _ctrlPressed = false;
		};

	}
}
