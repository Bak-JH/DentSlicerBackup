#pragma once

#include <unordered_set>
#include "interfaces/Feature.h"
#include "interfaces/Mode.h"
#include <memory>
#include <QString>
#include "interfaces/DialogedMode.h"
#include "../Qml/components/ControlForwardInclude.h"

class QNetworkAccessManager;
class BonjourServiceBrowser;
class BonjourServiceResolver;

namespace Hix
{
	namespace Features
	{
		//class PrinterServer : public Hix::Features::InstantMode
		class PrinterServer : public Hix::Features::DialogedMode
		{
		public:
			PrinterServer();
			virtual ~PrinterServer();
			void applyButtonClicked()override;

		private:
			void refresh();
			Hix::QML::Controls::DropdownBox* _printersDrop;
			Hix::QML::Controls::Button* _refreshButton;

			std::unique_ptr<QNetworkAccessManager> _manager;
			std::unique_ptr<BonjourServiceBrowser> _bonjourBrowser;
			std::unique_ptr<BonjourServiceResolver> _bonjourResolver;
		};


	}
}
