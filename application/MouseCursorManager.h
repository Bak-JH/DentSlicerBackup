#pragma once
#include <QCursor>

namespace Hix
{


	namespace Application
	{
		enum class CursorType
		{
			Hand,
			ClosedHand,
			Eraser,
			Default
		};
		class MouseCursorManager
		{
		public:
			MouseCursorManager();

			MouseCursorManager(const MouseCursorManager& other) = delete;
			MouseCursorManager(MouseCursorManager&& other) = delete;
			MouseCursorManager& operator=(MouseCursorManager other) = delete;
			MouseCursorManager& operator=(MouseCursorManager&& other) = delete;
			CursorType cursor()const;
			void setCursor(CursorType cursor);
		private:
			QCursor _cursorEraser;
			CursorType _currentCursor;

		};






	}
}
