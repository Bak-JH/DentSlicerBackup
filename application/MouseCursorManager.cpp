#include "MouseCursorManager.h"
#include <qpixmap.h>
#include <qapplication.h>
using namespace Hix::Application;

Hix::Application::MouseCursorManager::MouseCursorManager(): _cursorEraser(QPixmap(":/Resource/cursor_eraser.png")), _currentCursor(CursorType::Default)
{
}

CursorType Hix::Application::MouseCursorManager::cursor() const
{
	return CursorType();
}

void Hix::Application::MouseCursorManager::setCursor(CursorType cursor)
{
	if (_currentCursor != cursor)
	{
		_currentCursor = cursor;
		switch (_currentCursor)
		{
		case Hix::Application::CursorType::Hand:
			QApplication::setOverrideCursor(QCursor(Qt::PointingHandCursor));
			break;
		case Hix::Application::CursorType::ClosedHand:
			QApplication::setOverrideCursor(QCursor(Qt::ClosedHandCursor));
			break;
		case Hix::Application::CursorType::Eraser:
			QApplication::setOverrideCursor(_cursorEraser);
			break;
		case Hix::Application::CursorType::Default:
			QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));
			break;
		default:
			break;
		}
	}
}

