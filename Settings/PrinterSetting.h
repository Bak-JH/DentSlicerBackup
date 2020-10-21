#pragma once
#include <string>
#include "JSONParsedSetting.h"
#include "../DentEngine/src/Bounds3D.h"
#include <optional>
namespace Hix
{
	namespace Settings
	{
		class PrinterSetting:public JSONParsedSetting
		{
		public:
			//TODO:tmp
			PrinterSetting();
			enum class InfoFileType
			{
				Hix,
				ThreeDelight
			};
			enum class SliceImageType
			{
				SVG,
				PNG
			};
			enum class BedShape
			{
				Circle,
				Rect
			};
			//struct like
			std::string presetName;
			InfoFileType infoFileType;
			size_t sliceImageResolutionX;
			size_t sliceImageResolutionY;
			double screenX;
			double screenY;
			BedShape bedShape;
			double bedX;
			double bedY;
			double bedRadius;
			double bedOffsetX;
			double bedOffsetY;
			double bedHeight;
			Hix::Engine3D::Bounds3D bedBound;
			std::optional<rapidjson::Value> printerConstants;
			double pixelPerMMX()const;
			double pixelPerMMY()const;
			double pixelSizeX()const;
			double pixelSizeY()const;
		protected:
			std::array<char, 100> _buffer;
			rapidjson::MemoryPoolAllocator<> _allocator;
			void initialize()override;
			void parseJSONImpl(const rapidjson::Document& doc)override;

		};
	}
}
