#pragma once
#include <qquickwindow.h>
namespace Hix
{
	namespace QML
	{


		class MainWindow : public QQuickWindow
		{
		public:
			MainWindow(QWindow* parent = nullptr);
			virtual ~MainWindow();

		protected:
			void keyPressEvent(QKeyEvent* e) override;
			void keyReleaseEvent(QKeyEvent* e) override;


		private:


		};
	}
}