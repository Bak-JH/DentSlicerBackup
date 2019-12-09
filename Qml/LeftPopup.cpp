#include "LeftPopup.h"


Hix::QML::CloseButton::CloseButton(QQuickItem* parent) : _mouseArea(new QQuickMouseArea(this))
{
	setParent(parent);
	connect(_mouseArea, &QQuickMouseArea::clicked, this, &CloseButton::onClick);
	qvariant_cast<QObject*>(
		_mouseArea->property("anchors")
		)->setProperty("fill", _mouseArea->property("parent"));
}

Hix::QML::RoundButton::RoundButton(QQuickItem* parent) : _mouseArea(new QQuickMouseArea(this))
{
	setParent(parent);
	_mouseArea->setHoverEnabled(true);
	_btncolor = "#00b9c8";
	qvariant_cast<QObject*>(
		_mouseArea->property("anchors")
		)->setProperty("fill", _mouseArea->property("parent"));

	connect(_mouseArea, &QQuickMouseArea::clicked, this, &RoundButton::onClick);
	connect(_mouseArea, &QQuickMouseArea::entered, this, &RoundButton::onEntered);
	connect(_mouseArea, &QQuickMouseArea::exited, this, &RoundButton::onExited);
}


void Hix::QML::CloseButton::onClick()
{
	qDebug() << "close clicked";
}

void Hix::QML::RoundButton::onClick()
{
	qDebug() << "round button clicked";
}

void Hix::QML::RoundButton::onEntered()
{
	qDebug() << "round button entered";
	_btncolor.setNamedColor("#21959E");
	emit btnColorChanged(_btncolor);
	qDebug() << _btncolor;
	//_mouseArea->setProperty("color", "#21959E");
	//qvariant_cast<QObject*>(
	//	_mouseArea->property("parent")
	//	)->setProperty("color", _mouseArea->property("#21959E"));
}
void Hix::QML::RoundButton::onExited()
{
	qDebug() << "round button exited";
	_btncolor = "#00b9c8";
	emit btnColorChanged(_btncolor);
	//qDebug() << _btncolor;

}

void Hix::QML::LeftPopupShell::titleChanged()
{
	qDebug() << _title;
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

void Hix::QML::RoundButton::btntextChanged()
{
	qDebug() << _btntext;
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



Hix::QML::FlatCutButton::FlatCutButton(QQuickItem* parent) : _mouseArea(new QQuickMouseArea(this))
{
	connect(_mouseArea, &QQuickMouseArea::clicked, this, &FlatCutButton::onClick);
	qvariant_cast<QObject*>(
		_mouseArea->property("anchors")
		)->setProperty("fill", _mouseArea->property("parent"));
}

void Hix::QML::FlatCutButton::onClick()
{
	qDebug() << "flat clicked";
}



Hix::QML::FreeCutButton::FreeCutButton(QQuickItem* parent) : _mouseArea(new QQuickMouseArea(this))
{
	setParent(parent);
	connect(_mouseArea, &QQuickMouseArea::clicked, this, &FreeCutButton::onClick);
	_mouseArea->setHoverEnabled(true);
	_btncolor = "#f6feff";
	qvariant_cast<QObject*>(
		_mouseArea->property("anchors")
		)->setProperty("fill", _mouseArea->property("parent"));

}

void Hix::QML::FreeCutButton::onClick()
{
	qDebug() << "free clicked";
	//_btncolor.setNamedColor("#21959E");
	_btncolor.setNamedColor("#21959E");
	emit btnColorChanged(_btncolor);
	//_btncolor = "#21959e";
	//emit btnColorChanged(_btncolor);
	qDebug() << _btncolor;
}