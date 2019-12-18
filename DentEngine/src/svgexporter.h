#pragma once
#include <sstream>
#include <QDir>
#include <QJsonObject>
#include <QJsonDocument>
#include <QSvgRenderer>
#include <QImage>
#include <QPainter>
#include "polyclipping/polyclipping.h"

namespace Hix
{
	namespace Slicer
	{
		class Slices;

	}
}

class SVGexporter
{
public:
    void exportSVG(Hix::Slicer::Slices& shellSlices, QString outfoldername, bool isTemp);

private:
	void parsePolyTreeAndWrite(const ClipperLib::PolyNode* pn, bool isTemp, std::stringstream& content);
	void writePolygon(const ClipperLib::PolyNode * contour, bool isTemp, std::stringstream & content);
	void writePolygon(ClipperLib::Path & contour, bool isTemp, std::stringstream & content);
	void writeGroupHeader(int layer_idx, float z, std::stringstream & content);
	void writeGroupFooter(std::stringstream & content);
	void writeHeader(std::stringstream & content);
	void writeFooter(std::stringstream& content);

	float _ppmmX;
	float _ppmmY;
	float _resX;
	float _resY;

};

