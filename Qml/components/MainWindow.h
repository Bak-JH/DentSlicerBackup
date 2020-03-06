#pragma once
#include <qquickwindow.h>
#include "../../input/KeyboardController.h"
namespace Hix
{
	namespace QML
	{


		class MainWindow : public QQuickWindow
		{
		public:
			MainWindow(QWindow* parent = nullptr);
			virtual ~MainWindow();
			const Hix::Input::KeyboardController& keyboardHandler()const;
		protected:
			void keyPressEvent(QKeyEvent* e) override;
			void keyReleaseEvent(QKeyEvent* e) override;
		private:
			Hix::Input::KeyboardController _keyboardHandler;

		};
	}
}