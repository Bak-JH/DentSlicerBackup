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
			virtual ~KeyboardController();
			bool isMultiselect()const;
		private:
			void initMouseEvents();
			Qt3DInput::QKeyboardHandler * _keyboardHandler;

			bool _shiftPressed = false;
			bool _ctrlPressed = false;
			friend class KeyboardControllerLoader;
		};

		class KeyboardControllerLoader
		{
		private:
			static void init(KeyboardController& manager, Qt3DCore::QEntity* parentEntity);
			friend class Hix::Application::ApplicationManager;
		};

	}
}
