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
			SliceSetting();
			~SliceSetting();

			double layerHeight;
			SlicingMode slicingMode;
			bool useGPU;
			size_t AAXY;
			size_t AAZ;
			double minHeight;



		protected:
			void parseJSONImpl(const rapidjson::Document& doc)override;
			void initialize()override;
			rapidjson::Document doc()override;
		};
	}
}
