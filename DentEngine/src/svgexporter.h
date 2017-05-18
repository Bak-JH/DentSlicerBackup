#ifndef SVGEXPORTER_H
#define SVGEXPORTER_H
#include "polyclipping/clipper.hpp"
#include "configuration.h"
#include <fstream>

using namespace ClipperLib;
using namespace std;

class SVGexporter
{
public:
    SVGexporter(Configuration* cfg) : cfg(cfg) {}
    Configuration* cfg;

    void exportSVG(vector<Paths> contourLists, const char* outfilename);

    void writePolygon(ofstream& outfile, Path contour);
    void writeGroupHeader(ofstream& outfile, int layer_idx, float z);
    void writeGroupFooter(ofstream& outfile);
    void writeHeader(ofstream& outfile);
    void writeFooter(ofstream& outfile);
};

#endif // SVGEXPORTER_H
