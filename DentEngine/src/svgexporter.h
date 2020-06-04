#pragma once

#include "polyclipping/polyclipping.h"
#include "slicer.h"
#include "../../Settings/SliceSetting.h"
#include "../../Settings/PrinterSetting.h"
#include "mesh.h"



namespace Hix
{
	namespace Slicer
	{
		class SVGexporter
		{
		public:
			SVGexporter(float layerH, float ppmmX, float ppmmY, float resX, float resY, QVector2D offsetXY, bool invertX, QString outfoldername, Hix::Settings::SliceSetting::SlicingMode sm);
			void exportSVG(std::vector<Hix::Slicer::LayerGroup>& shellSlices);

		private:
			void parsePolyTreeAndWrite(const ClipperLib::PolyNode* pn, std::stringstream& content);
			void writePolygon(const ClipperLib::PolyNode* contour, std::stringstream& content);
			void writePolygon(ClipperLib::Path& contour, std::stringstream& content);
			void writeGroupHeader(int layer_idx, float z, std::stringstream& content);
			void writeGroupFooter(std::stringstream& content);
			void writeHeader(std::stringstream& content);
			void writeFooter(std::stringstream& content);

			float _layerHeight;
			float _ppmmX;
			float _ppmmY;
			float _resX;
			float _resY;
			QVector2D _offsetXY;
			bool _invertX = true;
			QString _outfoldername;
			Hix::Settings::SliceSetting::SlicingMode _slicingMode;


		};

	}
}


