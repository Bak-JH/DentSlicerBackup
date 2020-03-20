#pragma once
#include <sstream>
#include <QDir>
#include <QJsonObject>
#include <QJsonDocument>
#include <QSvgRenderer>
#include <QImage>
#include <QPainter>
#include "polyclipping/polyclipping.h"
#include "slicer.h"

class SVGexporter
{
public:
    void exportSVG(std::vector<Hix::Slicer::Slice>& shellSlices, QString outfoldername, bool isTemp);

private:
	void parsePolyTreeAndWrite(const ClipperLib::PolyNode* pn, std::stringstream& content);
	void writePolygon(const ClipperLib::PolyNode * contour, std::stringstream & content);
	void writePolygon(ClipperLib::Path & contour, std::stringstream & content);
	void writeGroupHeader(int layer_idx, float z, std::stringstream & content);
	void writeGroupFooter(std::stringstream & content);
	void writeHeader(std::stringstream & content);
	void writeFooter(std::stringstream& content);

	float _ppmmX;
	float _ppmmY;

	QVector2D _offsetXY;
	bool _invert = true;
	float _resX;
	float _resY;

};

