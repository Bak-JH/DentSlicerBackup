#pragma once
#include "PrinterSetting.h"
#include "DeployInfo.h"
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
				Uniform,
				Adaptive
			};

			SliceSetting();
			~SliceSetting();
			float layerHeight;
			SlicingMode slicingMode;
			bool invertX;
			void setLayerHeight(float);
			void setSliceMode(SlicingMode);
			void setInvertX(bool);

		protected:
			void parseJSONImpl(const rapidjson::Document& doc)override;
			void initialize()override;
			const std::filesystem::path& jsonPath()override;
			rapidjson::Document doc()override;
		};
	}
}
