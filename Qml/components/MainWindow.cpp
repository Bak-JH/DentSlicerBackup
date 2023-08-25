#include "MainWindow.h"
#include "../../application/ApplicationManager.h"
#include <QtQuick/private/qquickevents_p_p.h>
using namespace  Hix::QML;
//class QQuickCloseEvent;

Hix::QML::MainWindow::MainWindow(QWindow* parent)
{
	//QObject::connect(this, &QQuickWindow::closing, [this](QQuickCloseEvent* close) {
	//	QMetaObject::invokeMethod(this, []() {
	//		QCoreApplication::quit();
	//		}, Qt::QueuedConnection);

	//	
	//});
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
	//if (!Hix::Application::ApplicationManager::getInstance().featureManager().isFeatureActive())
	//{
		_keyboardHandler.keyPressed(e);
	//}
}

void Hix::QML::MainWindow::keyReleaseEvent(QKeyEvent* e)
{
	__super::keyReleaseEvent(e);
	//if (!Hix::Application::ApplicationManager::getInstance().featureManager().isFeatureActive())
	//{
		_keyboardHandler.keyReleased(e);
	//}

}
