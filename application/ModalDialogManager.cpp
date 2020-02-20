#include "ModalDialogManager.h"
#include "ApplicationManager.h"
#include "qmlmanager.h"

using namespace Hix::QML;
using namespace Hix::Application;
const QUrl POPUP_URL = QUrl("qrc:/Qml/ModalWindow.qml");

Hix::Application::ModalDialogManager::ModalDialogManager()
{
}

void Hix::Application::ModalDialogManager::openOkCancelDialog(const std::string& msg, const std::string& okButtonStr, const std::string& cancelButtonStr, 
	std::function<void()>&& okFunctor, const std::function<void()>&& cancelFunctor)
{
	QQmlComponent* component = new QQmlComponent(&Hix::Application::ApplicationManager::getInstance().engine(), POPUP_URL);
	auto qmlInstance = component->create(qmlContext(qmlManager->popupArea));
	auto popupShell = dynamic_cast<Hix::QML::ModalShell*>(qmlInstance);
	std::deque<ModalShellButtonArg> buttonArgs{
	{okButtonStr, QColor("#00b9c8"), QColor("#21959e"), okFunctor},
	{cancelButtonStr, QColor("#abb3b3"), QColor("#8b9393"), cancelFunctor},
	};	
	_popup.reset(popupShell);
	_popup->setButtonArgs(msg, std::move(buttonArgs));
	_popup->setParentItem(qmlManager->popupArea);
}

void Hix::Application::ModalDialogManager::closeDialog()
{
}

void Hix::Application::ModalDialogManagerLoader::init(ModalDialogManager& manager, QQuickItem* entity)
{
	manager._root = entity;
}
