#include "ModalDialogManager.h"
#include "ApplicationManager.h"
#include "Qml/components/Buttons.h"

using namespace Hix::QML;
using namespace Hix::Application;
const QUrl POPUP_URL = QUrl("qrc:/Qml/ModalWindow.qml");

Hix::Application::ModalDialogManager::ModalDialogManager()
{
}

void Hix::Application::ModalDialogManager::openOkCancelDialog(const std::string& msg, const std::string& okButtonStr, const std::string& cancelButtonStr, 
	std::function<void()>&& okFunctor, std::function<void()>&& cancelFunctor)
{
	//do not move, but copy cancel functor as it needs to be used twice
	std::deque<ModalShellButtonArg> buttonArgs{
	{okButtonStr, QColor("#00b9c8"), QColor("#21959e"), std::move(okFunctor)},
	{cancelButtonStr, QColor("#abb3b3"), QColor("#8b9393"), cancelFunctor}};	
	openDialog(msg, std::move(buttonArgs), std::move(cancelFunctor));
}

void Hix::Application::ModalDialogManager::openOkDialog(const std::string& msg, const std::string& okButtonStr, std::function<void()>&& okFunctor)
{
	std::deque<ModalShellButtonArg> buttonArgs{
	{okButtonStr, QColor("#00b9c8"), QColor("#21959e"), std::move(okFunctor)}};
	std::function<void()> defaultCancel = [this]() {
		closeDialog();
	};
	openDialog(msg, std::move(buttonArgs), std::move(defaultCancel));
}

void Hix::Application::ModalDialogManager::openCancelDialog(const std::string& msg, const std::string& cancelButtonStr, std::function<void()>&& cancelFunctor)
{
	std::deque<ModalShellButtonArg> buttonArgs{
	{cancelButtonStr, QColor("#00b9c8"), QColor("#21959e"),cancelFunctor} };
	openDialog(msg, std::move(buttonArgs), std::move(cancelFunctor));
}

void Hix::Application::ModalDialogManager::openDialog(const std::string& msg, std::deque<ModalShellButtonArg>&& args, std::function<void()>&& cancelFunctor)
{
	QQmlComponent* component = new QQmlComponent(&Hix::Application::ApplicationManager::getInstance().engine(), POPUP_URL);
	auto qmlInstance = component->create(qmlContext(_root));
	auto popupShell = dynamic_cast<Hix::QML::ModalShell*>(qmlInstance);
	_popup.reset(popupShell);
	_popup->setMessage(msg);
	_popup->addButton(std::move(args));
	_popup->setParentItem(_root);
	QObject::connect(&_popup->closeButton(), &Hix::QML::Controls::Button::clicked, std::move(cancelFunctor));
}

void Hix::Application::ModalDialogManager::needToSelectModels()
{
	openCancelDialog(
		"Need to select one or more models.", "Ok",
		[this]() {
			Hix::Application::ApplicationManager::getInstance().featureManager().setMode(nullptr);
			closeDialog();

		}
	);
}

void Hix::Application::ModalDialogManager::closeDialog()
{
	_popup.reset();
}

QQuickItem* Hix::Application::ModalDialogManager::popupArea()
{
	return _root;
}

void Hix::Application::ModalDialogManagerLoader::init(ModalDialogManager& manager, QQuickItem* entity)
{
	manager._root = entity;
}
