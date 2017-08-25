#ifndef SVGEXPORTER_H
#define SVGEXPORTER_H
#include "polyclipping/clipper.hpp"
#include "configuration.h"
#include "slicer.h"
#include <fstream>
#include <QDir>
#include <QJsonObject>
#include <QJsonDocument>

using namespace ClipperLib;
using namespace std;

class SVGexporter
{
public:
    SVGexporter(){};

    void exportSVG(Slices contourLists, QString outfoldername);

    void writePolygon(ofstream& outfile, Path contour);
    void writeGroupHeader(ofstream& outfile, int layer_idx, float z);
    void writeGroupFooter(ofstream& outfile);
    void writeHeader(ofstream& outfile);
    void writeFooter(ofstream& outfile);
};

#endif // SVGEXPORTER_H
