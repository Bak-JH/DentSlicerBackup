#pragma once
#include "PrinterSetting.h"
namespace Hix
{
	namespace Settings
	{
		class AppSetting
		{
		public:
			AppSetting();
			void refresh();
			void setPrinterPath(const std::string& path);
			const PrinterSetting& printerSetting()const;
		private:
			std::filesystem::path _printerFile;
			PrinterSetting _printerSetting;

		};
	}
}
