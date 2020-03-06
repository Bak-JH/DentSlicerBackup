#include "MainWindow.h"
#include "../../application/ApplicationManager.h"
using namespace  Hix::QML;

Hix::QML::MainWindow::MainWindow(QWindow* parent)
{
}

Hix::QML::MainWindow::~MainWindow()
{
}

const Hix::Input::KeyboardController& Hix::QML::MainWindow::keyboardHandler() const
{
	return _keyboardHandler;
}

void Hix::QML::MainWindow::keyPressEvent(QKeyEvent* e)
{
	__super::keyPressEvent(e);
	//activate only when there is no focus
	if (!Hix::Application::ApplicationManager::getInstance().featureManager().isFeatureActive())
	{
		_keyboardHandler.keyPressed(e);
	}
}

void Hix::QML::MainWindow::keyReleaseEvent(QKeyEvent* e)
{
	__super::keyReleaseEvent(e);
	if (!Hix::Application::ApplicationManager::getInstance().featureManager().isFeatureActive())
	{
		_keyboardHandler.keyReleased(e);
	}

}
