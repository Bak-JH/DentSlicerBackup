#include "svgexporter.h"
#include <QSvgRenderer>
#include <QImage>
#include <QPainter>
#include <sstream>
#include <QDir>
#include <QJsonObject>
#include <QJsonDocument>
#include "../common/rapidjson/stringbuffer.h"
#include "../common/rapidjson/writer.h"
#include "../common/rapidjson/ostreamwrapper.h"
#include "../common/rapidjson/prettywriter.h"
#include "../polyclipping/polyclipping.h"
#include "../../Settings/SliceSetting.h"
#include "../../Settings/PrinterSetting.h"
#include <fstream>

using namespace Hix::Slicer;
#if defined(_DEBUG) || defined(QT_DEBUG)
#define _DEBUG_SVG
#endif


SVGexporter::SVGexporter(float layerH, float ppmmX, float ppmmY, float resX, float resY, QVector2D offsetXY, bool isInvertX, 
    QString outfoldername, Hix::Settings::SliceSetting::SlicingMode sm):
    _layerHeight(layerH), _ppmmX(ppmmX), _ppmmY(ppmmY), _resX(resX), _resY(resY), _offsetXY(offsetXY), _invertX(isInvertX), _outfoldername(outfoldername), _slicingMode(sm)
{
}

void SVGexporter::exportSVG(std::vector<LayerGroup>& layerGroup){

    qDebug() << "export svg at "<< _outfoldername;
	qDebug() << "shellSlices : " << layerGroup.size();
    //qDebug() << jsonBytes;
    int currentSlice_idx = 0;
    for (int i=0; i< layerGroup.size(); i++){
        QString outfilename = _outfoldername + "/" + QString::number(currentSlice_idx) + ".svg";
        QFile outfile(outfilename);
        std::stringstream contentStream;
        outfile.open(QFile::WriteOnly);
        writeHeader(contentStream);
        //if (setting.slicingMode == Hix::Settings::SliceSetting::SlicingMode::Uniform)
        writeGroupHeader(currentSlice_idx, _layerHeight *(currentSlice_idx+1), contentStream);
        for (auto& s : layerGroup[i].slices)
        {
            PolyTree& shellSlice_polytree = *s.polytree;
            for (int j = 0; j < shellSlice_polytree.ChildCount(); j++) {
                parsePolyTreeAndWrite(shellSlice_polytree.Childs[j], contentStream);
            }
        }
		writeGroupFooter(contentStream);
        writeFooter(contentStream);
        outfile.write(QByteArray::fromStdString(contentStream.str()));

        outfile.close();
        currentSlice_idx += 1;
    }
    //printf("slicing done\n");
   
    //fflush(stdout);
    qDebug() << "slicing done";








}



void SVGexporter::parsePolyTreeAndWrite(const PolyNode* pn, std::stringstream& content){
    writePolygon(pn, content);
    for (int i=0; i<pn->ChildCount(); i++){
        PolyNode* new_pn = pn->Childs[i];
        parsePolyTreeAndWrite(new_pn, content);
    }

    /*while (pn != NULL){
        //area += Area(pn->Contour);

        pn = pn->GetNext();
    }*/

}

void SVGexporter::writePolygon(const PolyNode* contour, std::stringstream& content){
    content << "      <polygon contour:type=\"contour\" points=\"";
    for (IntPoint point: contour->Contour){
		if (_invertX)
		{
			point.X = -1 * point.X;
		}
		auto fp = Hix::Polyclipping::toFloatPt(point) + _offsetXY;
        content << std::fixed << 
			fp.x()*_ppmmX
			+ (_resX/2)
			<< "," << std::fixed <<
			_resY/2
			- fp.y()*_ppmmY<< " "; // doesn't need 100 actually// TODO fix this

    }
    if (! contour->IsHole()){
        content << "\" style=\"fill: white\" />\n";
    } else {
        content << "\" style=\"fill: black\" />\n";
    }
}

void SVGexporter::writePolygon(ClipperLib::Path& contour, std::stringstream& content){
    content << "      <polygon contour:type=\"contour\" points=\"";
    for (IntPoint point: contour){
		if (_invertX)
		{
			point.X = -1 * point.X;
		}
		auto fp = Hix::Polyclipping::toFloatPt(point) + _offsetXY;
        content << std::fixed << 
			fp.x() * _ppmmX
			+ (_resX/2) 
			<< ","<< std::fixed <<
			_resY/2
			- fp.y() * _ppmmY << " "; // doesn't need 100 actually

    }
    content << "\" style=\"fill: white\" />\n";
}

void SVGexporter:: writeGroupHeader(int layer_idx, float z, std::stringstream& content){
    content << "    <g id=\"layer" << layer_idx << "\" contour:z=\""<< std::fixed << z << "\">\n";
}

void SVGexporter:: writeGroupFooter(std::stringstream& content){
    content << "    </g>\n";
}

void SVGexporter::writeHeader(std::stringstream& content){
    content << "<svg width='" << _resX << "' height='" << _resY << "' xmlns='http://www.w3.org/2000/svg' xmlns:contour='http://hix.co.kr' style='background-color: black;'>\n";
	content << "<rect width='" << _resX << "' height='" << _resY << "' fill='black'>\n";
	content << "</rect>\n";

}

void SVGexporter::writeFooter(std::stringstream& content){
    content << "</svg>";
}

