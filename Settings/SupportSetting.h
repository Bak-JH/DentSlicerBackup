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
				Vertical
			};
			enum class RaftType : uint8_t
			{
				General
			};
			enum class InterconnectType : uint8_t
			{
				None,
				//Automatic,
				ZigZag,
				Cross
			};

			SupportSetting();
			~SupportSetting();

			SupportType supportType;
			RaftType raftType;
			double supportRadiusMax;
			double supportRadiusMin;
			double supportDensity;
			double supportBaseHeight;
			double raftThickness;
			double raftRadiusMult;
			double raftMinMaxRatio;
			InterconnectType interconnectType;
			double maxConnectDistance;
			bool thickenFeet;

		protected:
			void parseJSONImpl(const rapidjson::Document& doc)override;
			void initialize()override;
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
