#ifndef SVGEXPORTER_H
#define SVGEXPORTER_H
#include "polyclipping/clipper/clipper.hpp"
#include "configuration.h"
#include "slicer.h"
#include <fstream>
#include <QDir>
#include <QJsonObject>
#include <QJsonDocument>
#include <QSvgRenderer>
#include <QImage>
#include <QPainter>

using namespace ClipperLib;

class SVGexporter
{
public:
    SVGexporter(){};

    QString exportSVG(Slices shellSlices, Slices supportSlices, Slices raftSlices, QString outfoldername);

    void parsePolyTreeAndWrite(PolyNode* pn, std::ofstream& outfile);

    void writePolygon(std::ofstream& outfile, Path contour);
    void writePolygon(std::ofstream& outfile, PolyNode* contour);
    void writeGroupHeader(std::ofstream& outfile, int layer_idx, float z);
    void writeGroupFooter(std::ofstream& outfile);
    void writeHeader(std::ofstream& outfile);
    void writeFooter(std::ofstream& outfile);
};

#endif // SVGEXPORTER_H
