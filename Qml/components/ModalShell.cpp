#include "ModalShell.h"

#include "Buttons.h"
#include <QtQuick/private/qquickrectangle_p.h>
#include <QtQuick/private/qquicktext_p.h>
#include "QtQml/private/qqmllistmodel_p.h"
#include "../util/QMLUtil.h"
#include "glmodel.h"
using namespace  Hix::QML;
Hix::QML::ModalShell::ModalShell(QQuickItem* parent, const std::string& msg, std::deque<ModalShellButtonArg>&& args): QQuickItem(parent), _msg(QString::fromStdString(msg)), _buttonArgs(args)
{
}
Hix::QML::ModalShell::ModalShell(QQuickItem* parent) : QQuickItem(parent)
{
}

Hix::QML::ModalShell::~ModalShell()
{
}


void Hix::QML::ModalShell::addButton(const std::string& buttonName, const QColor& color, const QColor& highlightColor, std::function<void()>&& functor)
{
	auto rect = new QQuickRectangle();
	rect->setProperty("btnText", 1);
	rect->setProperty("defaultColor", color);
	rect->setProperty("hoverColor", highlightColor);
	rect->setParentItem(_buttonArea);
}

//void Hix::QML::ModalShell::setMessage(std::string message)
//{
//	_modalmsg = QString::fromStdString(message);
//	emit modalmsgChanged();
//}

void Hix::QML::ModalShell::componentComplete()
{
	__super::componentComplete();
	registerOwningControls();
	getItemByID(this, _buttonArea, "buttonarea");
	for (auto& arg: _buttonArgs)
	{
		addButton(arg.buttonName, arg.defaultColor, arg.hoverColor, std::move(arg.onClickFunctor));
	}
}

QQuickItem* Hix::QML::ModalShell::getQItem()
{
	return this;
}



