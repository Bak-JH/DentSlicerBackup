#pragma once
#include <string>
#include <qcolor.h>
#include <functional>
#include <memory>
#include "../Qml/components/ModalShell.h"

class QQuickItem;

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
			ModalDialogManager();
			ModalDialogManager(const ModalDialogManager& other) = delete;
			ModalDialogManager(ModalDialogManager&& other) = delete;
			ModalDialogManager& operator=(ModalDialogManager other) = delete;
			ModalDialogManager& operator=(ModalDialogManager&& other) = delete;

			void openOkCancelDialog(const std::string& msg, const std::string& okButtonStr,
				const std::string& cancelButtonStr, std::function<void()>&& okFunctor, const std::function<void()>&& cancelFunctor);
			void closeDialog();
		private:
			std::unique_ptr<Hix::QML::ModalShell> _popup;
			QQuickItem* _root;
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
