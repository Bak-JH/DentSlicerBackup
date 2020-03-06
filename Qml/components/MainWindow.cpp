#include "MainWindow.h"

using namespace  Hix::QML;

Hix::QML::MainWindow::MainWindow(QWindow* parent)
{
}

Hix::QML::MainWindow::~MainWindow()
{
}

void Hix::QML::MainWindow::keyPressEvent(QKeyEvent* e)
{
	qDebug() << "press window";
	__super::keyPressEvent(e);
}

void Hix::QML::MainWindow::keyReleaseEvent(QKeyEvent* e)
{
	qDebug() << "release window";
	__super::keyReleaseEvent(e);

}
