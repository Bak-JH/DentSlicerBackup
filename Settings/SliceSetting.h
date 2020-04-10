#pragma once
#include "JSONParsedSetting.h"
#include "JSONWriteSetting.h"
namespace Hix
{
	namespace Settings
	{
		class SliceSetting:public JSONParsedSetting, public JSONWriteSetting
		{
		public:
			enum class SlicingMode : uint8_t
			{
				Uniform
				//,Adaptive
			};

			SliceSetting(std::filesystem::path settingsPath);
			~SliceSetting();

			float layerHeight;
			SlicingMode slicingMode;
			bool invertX;

		protected:
			void parseJSONImpl(const rapidjson::Document& doc)override;
			void initialize()override;
			const std::filesystem::path& jsonPath()override;
			rapidjson::Document doc()override;
		};
	}
}
