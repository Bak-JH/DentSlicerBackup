#pragma once
#include "JSONParsedSetting.h"
#include "JSONWriteSetting.h"


namespace Hix
{
	namespace Settings
	{


		class LabelSetting :public JSONParsedSetting, public JSONWriteSetting
		{
		public:
			LabelSetting();
			~LabelSetting();

			std::string labelFontFamily;
			double labelFontSize = 40.0;
			double labelDepth = 0.2;
			bool isEmboss = false;

		protected:
			void parseJSONImpl(const rapidjson::Document& doc)override;
			void initialize()override;
			rapidjson::Document doc()override;


		};
	}
}
