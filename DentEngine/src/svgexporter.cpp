#include "svgexporter.h"

#include "../common/rapidjson/stringbuffer.h"
#include "../common/rapidjson/writer.h"
#include "../common/rapidjson/ostreamwrapper.h"
#include "../common/rapidjson/prettywriter.h"

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

void Hix::Slicer::SVGexporter::createInfoFile()
{
    _infoJsonName = _outfoldername + "/" + "info.json";
    QFile infofile(_infoJsonName);
    infofile.open(QFile::WriteOnly);

    //std::ofstream infofile(infofilename.toStdString().c_str(), std::ios::out);
    QJsonObject jsonObject;

    QJsonDocument jsonDocument(jsonObject);
    QByteArray jsonBytes = jsonDocument.toJson();
    infofile.write(jsonBytes);
    infofile.close();
}


void SVGexporter::writeVittroOptions(int max_slices, const Hix::Engine3D::Bounds3D& bound){

    // write buildscript.ini file
    QString buildscriptfilename = _outfoldername + "/" + "buildscript.ini";
    QFile buildscriptfile(buildscriptfilename);
    buildscriptfile.open(QFile::WriteOnly);
    buildscriptfile.write(QString("Machine = Vittro Plus 6\r\n\
Slice thickness = %1\r\n\
number of slices = %2\r\n\
illumination time = 6\r\n\
number of override slices = 0\r\n\
override illumination time = 10\r\n\
build time estimation = 2718\r\n\
material consumption estimation = 29.9781\r\n").arg(QString::number((int)(_layerHeight *1000)), QString::number(max_slices)).toStdString().data());


    // do run svg 2 png
	for (int i=0; i<max_slices; i++){
        QString svgfilename = _outfoldername + "/" + QString::number(i) + ".svg";
        QSvgRenderer renderer(svgfilename);
        QImage image(_resX, _resY, QImage::Format_RGB32);
        image.fill(0x000000);
        QPainter painter(&image);
        renderer.render(&painter);
        QString savesvgfilename = "S" + QString::number(i).rightJustified(6,'0') + "_P1.png";
        image.save(_outfoldername + "/" + savesvgfilename);
        buildscriptfile.write(QString("%1, %2, %3\r\n").arg(QString::number(i*_layerHeight), savesvgfilename, "6.0").toStdString().data());
    }
    buildscriptfile.close();


    // write  parameters.ini file
    QString parametersfilename = _outfoldername + "/" + "parameters.ini";
    QFile parametersfile(parametersfilename);
    parametersfile.open(QFile::WriteOnly);
    parametersfile.write(QString("[SELECTED PROFILES]\r\n\
Material = Detax\r\n\
Build Strategy = %1um\r\n\
[MACHINE SETTINGS]\r\n\
Platform size X = %2\r\n\
Platform size Y = %3\r\n\
Platform size Z = 100\r\n\
Image Format = 8 bit PNG\r\n\
Image size in X = %4\r\n\
Image size in Y = %5\r\n\
Mirror X = false\r\n\
Mirror Y = false\r\n\
Export Mode = Encrypted ZIP\r\n\
[SLICING]\r\n\
Layer Thickness = %6\r\n\
[CURING]\r\n\
Normal curing time = 6\r\n\
Number of first layers = 0\r\n\
First layers curing time = 10\r\n\
[IMAGE POST PROCESSING]\r\n\
Method = Anti-Aliasing 5X\r\n\
[SCALING]\r\n\
Scale in X = 1\r\n\
Scale in Y = 1\r\n\
Scale in Z = 1\r\n\
[BASE PLATE]\r\n\
Type = None\r\n\
Height = 0.6\r\n\
[NON SOLID SUPPORT]\r\n\
Line Thickness = 1\r\n\
[SCAFFOLDING SUPPORT]\r\n\
Enable support generation = true\r\n\
Critical angle = 30\r\n\
Suction force = 10\r\n\
Influence region interior = 1\r\n\
Influence region border = 1\r\n\
Max overhang = 1\r\n\
XY offset = 0.2\r\n\
Contact width = 0.2\r\n\
Contact margin = 0.2\r\n\
Penetration = 0.25\r\n\
Connection width = 1\r\n\
Diamond width = 3\r\n\
Diamond angle = 45\r\n\
Edge width = 1\r\n\
Edge thickness = 1\r\n\
Distance to Part = 1\r\n\
Max offset from Part = -1\r\n\
Grid Base Plate Type = None").arg(QString::number((int)(_layerHeight*1000)),
            QString::number(bound.lengthX()), QString::number(bound.lengthY()),
            QString::number(_resX), QString::number(_resY),
            QString::number(_layerHeight)).toStdString().data());
    parametersfile.close();

}

void Hix::Slicer::SVGexporter::writeBasicInfo(int sliceCnt, const std::optional<rapidjson::Value>& optionalVal)
{
    char cbuf[1024]; rapidjson::MemoryPoolAllocator<> allocator(cbuf, sizeof cbuf);
    rapidjson::Document doc(&allocator, 256);
    doc.SetObject();
    doc.AddMember("layer_height", _layerHeight, allocator);
    doc.AddMember("total_layer", sliceCnt, allocator);
    if (optionalVal)
    {
        auto obj = optionalVal.value().GetObjectW();
        for (auto itr = obj.MemberBegin(); itr != obj.MemberEnd(); ++itr)
        {
            rapidjson::Value val, key;
            key.CopyFrom(itr->name, allocator);;
            val.CopyFrom(itr->value, allocator);;
            doc.AddMember(std::move(key), std::move(val), allocator);
        }
    }

    std::ofstream of(_infoJsonName.toStdU16String(), std::ios_base::trunc);
    rapidjson::OStreamWrapper osw{ of };
    rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer{ osw };
    doc.Accept(writer);
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

