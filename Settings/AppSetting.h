#pragma once
#include "PrinterSetting.h"
namespace Hix
{
	namespace Settings
	{
		class AppSetting:public JSONParsedSetting
		{
		public:
			AppSetting();
			~AppSetting();
			void refresh() override;
			void setPrinterPath(const std::string& path);
			bool enableErrorReport;
			std::string printerPresetPath;
			PrinterSetting printerSetting;
		protected:
			void initialize()override;
			void parseJSONImpl(const rapidjson::Document& doc)override;


		};
	}
}
