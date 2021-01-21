#pragma once
#include <string>
#include <qcolor.h>
#include <functional>
#include <memory>
#include "../Qml/components/ModalShell.h"

class QQuickItem;
class QQmlApplicationEngine;
namespace Hix
{
	namespace QML
	{
		//class ModalShell;
	}
	namespace Application
	{
		class ApplicationManager;
		class ModalDialogManager
		{
		public:
			ModalDialogManager(QQmlApplicationEngine* engine);
			ModalDialogManager(const ModalDialogManager& other) = delete;
			ModalDialogManager(ModalDialogManager&& other) = delete;
			ModalDialogManager& operator=(ModalDialogManager other) = delete;
			ModalDialogManager& operator=(ModalDialogManager&& other) = delete;

			void openOkCancelDialog(const std::string& msg, const std::string& okButtonStr,
				const std::string& cancelButtonStr, std::function<void()>&& okFunctor, std::function<void()>&& cancelFunctor);
			void openOkDialog(const std::string& msg, const std::string& okButtonStr,
				std::function<void()>&& okFunctor);
			void openCancelDialog(const std::string& msg, const std::string& cancelButtonStr,
				std::function<void()>&& cancelFunctor);
			void openDialog(const std::string& msg, std::deque<Hix::QML::ModalShellButtonArg>&& args, std::function<void()>&& cancelFunctor);

			//for modes
			void needToSelectModels();
			void closeDialog();
			QQuickItem* popupArea();
		private:
			std::unique_ptr<Hix::QML::ModalShell> _popup;
			QQuickItem* _root;
			QQmlComponent _component;

			friend class ModalDialogManagerLoader;
		};
		class ModalDialogManagerLoader
		{
		private:
			static void init(ModalDialogManager& manager, QQuickItem* entity);
			friend class Hix::Application::ApplicationManager;
		};




	}
}
