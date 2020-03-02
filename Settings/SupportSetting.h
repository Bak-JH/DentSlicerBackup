#pragma once
#include "JSONParsedSetting.h"
#include "JSONWriteSetting.h"
namespace Hix
{
	namespace Settings
	{
		class SupportSetting:public JSONParsedSetting, public JSONWriteSetting
		{
		public:

			enum class SupportType : uint8_t
			{
				None,
				Vertical
			};
			enum class RaftType : uint8_t
			{
				None,
				General
			};


			SupportSetting(std::filesystem::path settingsPath);
			~SupportSetting();

			SupportType supportType;
			RaftType raftType;
			float supportRadiusMax;
			float supportRadiusMin;
			float supportDensity;
			float supportBaseHeight;
			float raftThickness;


		protected:
			void parseJSONImpl(const rapidjson::Document& doc)override;
			void initialize()override;
			const std::filesystem::path& jsonPath()override;
			rapidjson::Document doc()override;
		};
	}
}
