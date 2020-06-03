#pragma once
#include "../../Settings/SliceSetting.h"
#include "../../Settings/PrinterSetting.h"
#include <QString>
#include "../common/rapidjson/rapidjson.h"
#include "../common/rapidjson/document.h"

namespace Hix
{
	namespace Slicer
	{
		//TODO: refactor this
		class InfoWriter
		{
		private:
			QString _outfoldername;
			QString _infoJsonName;
			size_t _resX, _resY;
			float _layerHeight;

		public:
			InfoWriter(QString outFolder, size_t resX, size_t resY, float layerHeight);
			void createInfoFile();
			void writeVittroOptions(int max_slices, const Hix::Engine3D::Bounds3D& bound);
			void writeBasicInfo(int sliceCnt, const std::optional<rapidjson::Value>& optionalVal);
		};
	}
}


