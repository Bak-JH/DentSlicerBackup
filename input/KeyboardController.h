#pragma once

namespace Qt3DInput
{
	class QKeyboardHandler;
	class QKeyEvent;
}
namespace Qt3DCore
{
	class QNode;
}
class QQuickItem;
namespace Hix
{
	namespace Input
	{
		class KeyboardController
		{
		public:
			KeyboardController(Qt3DCore::QNode* parent);
			virtual ~KeyboardController();
			bool isMultiselect()const;
		private:
			Qt3DInput::QKeyboardHandler* _keyboardHandler;
			bool _shiftPressed = false;
			bool _ctrlPressed = false;
		};


	}
}
