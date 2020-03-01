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

			//void setSupportType(SupportType val);
			//void setRaftType(RaftType val);

			//void setSupportRadiusMax(float val);
			//void setSupportRadiusMin(float val);
			//void setSupportDensity(float val);
			//void setSupportBaseHeight(float val);
			//void setRaftThickness(float val);


		protected:
			void parseJSONImpl(const rapidjson::Document& doc)override;
			void initialize()override;
			const std::filesystem::path& jsonPath()override;
			rapidjson::Document doc()override;
		};
	}
}
