#include "ModalDialogManager.h"
using namespace Hix::QML;
using namespace Hix::Application;
Hix::Application::ModalDialogManager::ModalDialogManager()
{
}

void Hix::Application::ModalDialogManager::openOkCancelDialog(const std::string& msg, const std::string& okButtonStr, const std::string& cancelButtonStr, 
	std::function<void()>&& okFunctor, const std::function<void()>&& cancelFunctor)
{
	std::deque<ModalShellButtonArg> buttonArgs{
	{okButtonStr, QColor("#00b9c8"), QColor("#21959e"), okFunctor},
	{cancelButtonStr, QColor("#abb3b3"), QColor("#8b9393"), cancelFunctor},
	};
	_popup.reset(new ModalShell(_root, msg, std::move(buttonArgs)));
}

void Hix::Application::ModalDialogManager::closeDialog()
{
}

QQuickItem* Hix::Application::ModalDialogManager::popupArea()
{
	return _root;
}

void Hix::Application::ModalDialogManagerLoader::init(ModalDialogManager& manager, QQuickItem* entity)
{
	manager._root = entity;
}
