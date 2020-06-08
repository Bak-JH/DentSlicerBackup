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
			float raftRadiusMult;
			float raftMinMaxRatio;

		protected:
			void parseJSONImpl(const rapidjson::Document& doc)override;
			void initialize()override;
			const std::filesystem::path& jsonPath()override;
			rapidjson::Document doc()override;
		};
	}
}

namespace magic_enum
{

	template<>
	struct name_formatter<Hix::Settings::SupportSetting::SupportType>
	{
		static constexpr name_option name_option = name_option::spaced_camel;
	};
	template<>
	struct name_formatter<Hix::Settings::SupportSetting::RaftType>
	{
		static constexpr name_option name_option = name_option::spaced_camel;
	};
}
