#pragma once
#include "JSONWriteSetting.h"


namespace Hix
{
	namespace Settings
	{


		class BasicSetting:public JSONParsedSetting, public JSONWriteSetting
		{
		public:
			BasicSetting();
			~BasicSetting();
			bool enableErrorReport;
			std::string printerPresetPath;

		protected:
			void parseJSONImpl(const rapidjson::Document& doc)override;
			void initialize()override;
			rapidjson::Document doc()override;


		};
	}
}
