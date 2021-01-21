#pragma once

#include <unordered_set>
#include "interfaces/Feature.h"
#include "interfaces/Mode.h"
#include <memory>
#include <QString>
#include "interfaces/DialogedMode.h"
#include "../Qml/components/ControlForwardInclude.h"
#include "../common/platform/Process.h"
class QNetworkAccessManager;
class BonjourServiceBrowser;
class BonjourServiceResolver;
class PrinterServerSetting;
class BonjourRecord;
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
			void checkIP(const QString& ip);
			void attachResolver(const BonjourRecord& record);
			Hix::QML::Controls::DropdownBox* _printersDrop;
			Hix::QML::Controls::Button* _refreshButton;
			Hix::Common::Process::Process _mdnsService;
			std::unique_ptr<QNetworkAccessManager> _manager;
			std::unique_ptr<BonjourServiceBrowser> _bonjourBrowser;
			std::vector<std::unique_ptr<BonjourServiceResolver>> _bonjourResolvers;
			std::unique_ptr<PrinterServerSetting> _printerServerSetting;
		};


	}
}
