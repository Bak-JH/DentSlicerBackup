#include "ModalShell.h"

#include "Buttons.h"

#include "../util/QMLUtil.h"
#include "glmodel.h"

#include "application/ApplicationManager.h"

using namespace  Hix::QML;
const QUrl ITEM_URL = QUrl("qrc:/Qml/ModalWindowButton.qml");

Hix::QML::ModalShell::ModalShell(QQuickItem* parent) : QQuickItem(parent), _component(&Hix::Application::ApplicationManager::getInstance().engine(), ITEM_URL)
{
#ifdef _DEBUG
	if (!_component.isReady())
	{
		qDebug() << "_component error: " << _component.errors();
	}
#endif
}

Hix::QML::ModalShell::~ModalShell()
{
}

//
//void Hix::QML::ModalShell::addButton(const std::string& buttonName, const QColor& color, const QColor& highlightColor, std::function<void()>&& functor)
//{
//	auto rect = new QQuickRectangle();
//	rect->setProperty("btnText", 1);
//	rect->setProperty("defaultColor", color);
//	rect->setProperty("hoverColor", highlightColor);
//	rect->setParentItem(_buttonArea);
//}

void Hix::QML::ModalShell::addButton(std::deque<ModalShellButtonArg>&& args)
{

	for (auto& arg : args)
	{
		auto button = dynamic_cast<Hix::QML::Controls::Button*>(_component.create(qmlContext(_buttonArea)));
		button->setProperty("btnText", QString::fromStdString(arg.buttonName));
		button->setProperty("defaultColor", arg.defaultColor);
		button->setProperty("hoverColor", arg.hoverColor);
		button->setProperty("width", (_buttonArea->width() - (32 * (args.size() - 1))) / args.size()); // DON'T EDIT THIS PART.
		button->setParentItem(_buttonArea);

		QObject::connect(button, &Hix::QML::Controls::Button::clicked, arg.onClickFunctor);
	}
}

void Hix::QML::ModalShell::setMessage(const std::string& message)
{
 	_msg = QString::fromStdString(message);
	emit modalmsgChanged();
}

Controls::Button& Hix::QML::ModalShell::closeButton()
{
	return *_closeButton;
}

void Hix::QML::ModalShell::componentComplete()
{
	__super::componentComplete();
	registerOwningControls();
	getItemByID(this, _buttonArea, "buttonarea");
	getControl(_closeButton, "closeButton");
}

QQuickItem* Hix::QML::ModalShell::getQItem()
{
	return this;
}



