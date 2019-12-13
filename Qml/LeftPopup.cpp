#include "LeftPopup.h"


Hix::QML::CloseButton::CloseButton(QQuickItem* parent) : _mouseArea(new QQuickMouseArea(this))
{
	setParent(parent);
	connect(_mouseArea, &QQuickMouseArea::clicked, this, &CloseButton::onClick);
	qvariant_cast<QObject*>(
		_mouseArea->property("anchors")
		)->setProperty("fill", _mouseArea->property("parent"));
}
void Hix::QML::CloseButton::onClick()
{
	qDebug() << "close clicked";
}



Hix::QML::RoundButton::RoundButton(QQuickItem* parent) :QQuickRectangle(parent), _mouseArea(new QQuickMouseArea(this))
{
	connect(_mouseArea, &QQuickMouseArea::clicked, this, &RoundButton::onClick);
	connect(_mouseArea, &QQuickMouseArea::entered, this, &RoundButton::onEntered);
	connect(_mouseArea, &QQuickMouseArea::exited, this, &RoundButton::onExited);

	_mouseArea->setHoverEnabled(true);
	qvariant_cast<QObject*>(
		_mouseArea->property("anchors")
		)->setProperty("fill", _mouseArea->property("parent"));
}


void Hix::QML::RoundButton::onClick()
{
	emit clicked();
}
void Hix::QML::RoundButton::onEntered()
{
	emit entered();
}
void Hix::QML::RoundButton::onExited()
{
	emit exited();
}
void Hix::QML::RoundButton::fNameChanged()
{
	emit fNameChanged();
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








void Hix::QML::LeftPopupShell::titleChanged()
{
	qDebug() << _title;
}

void Hix::QML::LeftPopupShell::bodyChanged()
{
	qDebug() << _body;
}

void Hix::QML::LeftPopupShell::popupHeightChanged()
{
	qDebug() << _popupHeight;
}

void Hix::QML::InputBox::propNameChanged()
{
	qDebug() << _propName;
}

void Hix::QML::DropdownBox::dropNameChanged()
{
	qDebug() << _dropName;
}


Hix::QML::LeftPopupShell::LeftPopupShell(QQuickItem* parent)
{
	setParent(parent);
}

Hix::QML::InputBox::InputBox(QQuickItem* parent):_inputRect(new QQuickSpinBox(this))
{
	setParent(parent);
}

Hix::QML::DropdownBox::DropdownBox(QQuickItem* parent):_dropRect(new QQuickComboBox(this))
{
	setParent(parent);
}

