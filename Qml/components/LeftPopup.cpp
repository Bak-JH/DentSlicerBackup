#include "LeftPopup.h"

using namespace  Hix::QML;

Hix::QML::LeftPopupShell::LeftPopupShell(QQuickItem* parent) : QQuickItem(parent)
{
}


//
//Hix::QML::TextButton::TextButton(QQuickItem* parent) : RoundButton(parent)
//{
//	_btncolor = "#00b9c8";
//	connect(_mouseArea, &QQuickMouseArea::clicked, this, &TextButton::onClick);
//	connect(_mouseArea, &QQuickMouseArea::entered, this, &TextButton::onEntered);
//	connect(_mouseArea, &QQuickMouseArea::exited, this, &TextButton::onExited);
//}
//
//
//void Hix::QML::TextButton::btntextChanged()
//{
//
//}
//
//
//void Hix::QML::TextButton::onClick()
//{
//	qDebug() << "round button clicked";
//}
//
//void Hix::QML::TextButton::onEntered()
//{
//	qDebug() << "round button entered";
//	_btncolor.setNamedColor("#21959E");
//	emit btnColorChanged(_btncolor);
//	qDebug() << _btncolor;
//	//_mouseArea->setProperty("color", "#21959E");
//	//qvariant_cast<QObject*>(
//	//	_mouseArea->property("parent")
//	//	)->setProperty("color", _mouseArea->property("#21959E"));
//}
//void Hix::QML::TextButton::onExited()
//{
//	qDebug() << "round button exited";
//	_btncolor = "#00b9c8";
//	emit btnColorChanged(_btncolor);
//	//qDebug() << _btncolor;
//
//}
