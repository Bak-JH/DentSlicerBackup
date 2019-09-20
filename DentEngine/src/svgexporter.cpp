#include "svgexporter.h"
#include "slicer.h"
#include "polyclipping/polyclipping.h"
#include "configuration.h"

using namespace ClipperLib;

#if defined(_DEBUG) || defined(QT_DEBUG)
#define _DEBUG_SVG
#endif

namespace SVGexporterPrivate
{
    void parsePolyTreeAndWrite(const ClipperLib::PolyNode* pn, bool isTemp, std::stringstream& content);
    void writePolygon(ClipperLib::Path& contour, bool isTemp, std::stringstream& content);
    void writePolygon(const ClipperLib::PolyNode* contour, bool isTemp, std::stringstream& content);
    void writeGroupHeader(int layer_idx, float z, std::stringstream& content);
    void writeGroupFooter(std::stringstream& content);
    void writeHeader(std::stringstream& content);
    void writeFooter(std::stringstream& content);
	void writeVittroOptions(QString outfoldername, int max_slices);

}


void SVGexporter::exportSVG(Slices& shellSlices,QString outfoldername, bool isTemp){
	using namespace SVGexporterPrivate;
    qDebug() << "export svg at "<< outfoldername;
	qDebug() << "shellSlices : " << shellSlices.size();
    QDir dir(outfoldername);
    if (!dir.exists()) {
        dir.mkpath(".");
    } else {
        dir.removeRecursively();
        dir.mkpath(".");
    }

    //qDebug() << jsonBytes;
    int currentSlice_idx = 0;
    for (int i=0; i<shellSlices.size(); i++){
        QString outfilename = outfoldername + "/" + QString::number(currentSlice_idx) + ".svg";
        QFile outfile(outfilename);
        std::stringstream contentStream;
		PolyTree& shellSlice_polytree = shellSlices[i].polytree;

		if (shellSlice_polytree.ChildCount() == 0) continue;

        outfile.open(QFile::WriteOnly);

        writeHeader(contentStream);
        if (scfg->slicing_mode == SlicingConfiguration::SlicingMode::Uniform)
            writeGroupHeader(currentSlice_idx, scfg->layer_height*(currentSlice_idx+1), contentStream);
        else
            writeGroupHeader(currentSlice_idx, scfg->layer_height*(currentSlice_idx+1), contentStream);
        
        for (int j=0; j<shellSlice_polytree.ChildCount(); j++){
            parsePolyTreeAndWrite(shellSlice_polytree.Childs[j], isTemp, contentStream);
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


	QString infofilename = outfoldername + "/" + "info.json";
	QFile infofile(infofilename);
	infofile.open(QFile::WriteOnly);

	//std::ofstream infofile(infofilename.toStdString().c_str(), std::ios::out);
	QJsonObject jsonObject;
	jsonObject["layer_height"] = round(scfg->layer_height * 100) / 100;
	jsonObject["total_layer"] = currentSlice_idx;
	jsonObject["bed_curing_time"] = 15000; // depends on scfg->resin_type
	jsonObject["curing_time"] = 2100; // depends on scfg->resin_type
	jsonObject["mirror_rot_time"] = 2000;
	jsonObject["z_hop_height"] = 15;
	jsonObject["move_up_feedrate"] = 350;
	jsonObject["move_down_feedrate"] = 500;
	jsonObject["resin_type"] = (uint8_t)scfg->resin_type;
	jsonObject["contraction_ratio"] = scfg->contraction_ratio;
	QJsonDocument jsonDocument(jsonObject);
	QByteArray jsonBytes = jsonDocument.toJson();
	infofile.write(jsonBytes);
	infofile.close();



    //exit(0);
	if (!isTemp && scfg->printer_vendor_type == SlicingConfiguration::PrinterVendor::ThreeDLight)
	{
		writeVittroOptions(outfoldername, currentSlice_idx);
	}


}


void SVGexporterPrivate::writeVittroOptions(QString outfoldername, int max_slices){

    // write buildscript.ini file
    QString buildscriptfilename = outfoldername + "/" + "buildscript.ini";
    QFile buildscriptfile(buildscriptfilename);
    buildscriptfile.open(QFile::WriteOnly);
    buildscriptfile.write(QString("Machine = Vittro Plus 6\r\n\
Slice thickness = %1\r\n\
number of slices = %2\r\n\
illumination time = 6\r\n\
number of override slices = 0\r\n\
override illumination time = 10\r\n\
build time estimation = 2718\r\n\
material consumption estimation = 29.9781\r\n").arg(QString::number((int)(scfg->layer_height*1000)), QString::number(max_slices)).toStdString().data());


    // do run svg 2 png

    for (int i=0; i<max_slices; i++){
        QString svgfilename = outfoldername + "/" + QString::number(i) + ".svg";
        QSvgRenderer renderer(svgfilename);
        QImage image(scfg->resolutionX(), scfg->resolutionY(), QImage::Format_RGB32);
        image.fill(0x000000);
        QPainter painter(&image);
        renderer.render(&painter);
        QString savesvgfilename = "S" + QString::number(i).rightJustified(6,'0') + "_P1.png";
        image.save(outfoldername + "/" + savesvgfilename);
        buildscriptfile.write(QString("%1, %2, %3\r\n").arg(QString::number(i*scfg->layer_height), savesvgfilename, "6.0").toStdString().data());
    }
    buildscriptfile.close();


    // write  parameters.ini file
    QString parametersfilename = outfoldername + "/" + "parameters.ini";
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
Grid Base Plate Type = None").arg(QString::number((int)(scfg->layer_height*1000)),
            QString::number(scfg->bedX()), QString::number(scfg->bedY()),
            QString::number(scfg->resolutionX()), QString::number(scfg->resolutionY()),
            QString::number(scfg->layer_height)).toStdString().data());
    parametersfile.close();

}



void SVGexporterPrivate::parsePolyTreeAndWrite(const PolyNode* pn, bool isTemp, std::stringstream& content){
    writePolygon(pn, isTemp, content);
    for (int i=0; i<pn->ChildCount(); i++){
        PolyNode* new_pn = pn->Childs[i];
        parsePolyTreeAndWrite(new_pn, isTemp, content);
    }

    /*while (pn != NULL){
        //area += Area(pn->Contour);

        pn = pn->GetNext();
    }*/

}

void SVGexporterPrivate::writePolygon(const PolyNode* contour, bool isTemp, std::stringstream& content){
    content << "      <polygon contour:type=\"contour\" points=\"";
    for (IntPoint point: contour->Contour){
		if (!isTemp && scfg->slice_invert == SlicingConfiguration::Invert::InvertXAxis)
		{
			point.X = -1 * point.X;
		}
		auto fp = Hix::Polyclipping::toFloatPt(point);
        content << std::fixed << 
			fp.x()*scfg->pixelPerMMX()/scfg->contraction_ratio
			+ (scfg->resolutionX()/2)
			<< "," << std::fixed <<
			scfg->resolutionY()/2
			- fp.y()*scfg->pixelPerMMX()/scfg->contraction_ratio << " "; // doesn't need 100 actually// TODO fix this

        // just fit to origin
        //outfile << std::fixed << (float)point.X/Hix::Polyclipping::INT_PT_RESOLUTION - scfg->origin.x() << "," << std::fixed << (float)point.Y/Hix::Polyclipping::INT_PT_RESOLUTION - scfg->origin.y() << " ";
    }
    if (! contour->IsHole()){
        content << "\" style=\"fill: white\" />\n";
    } else {
        content << "\" style=\"fill: black\" />\n";
    }
}

void SVGexporterPrivate::writePolygon(ClipperLib::Path& contour, bool isTemp, std::stringstream& content){
    content << "      <polygon contour:type=\"contour\" points=\"";
    for (IntPoint point: contour){
		if (!isTemp && scfg->slice_invert == SlicingConfiguration::Invert::InvertXAxis)
		{
			point.X = -1 * point.X;
		}
		auto fp = Hix::Polyclipping::toFloatPt(point);
        content << std::fixed << 
			fp.x() * scfg->pixelPerMMX()/scfg->contraction_ratio
			+ (scfg->resolutionX()/2) 
			<< ","<< std::fixed <<
			scfg->resolutionY()/2
			- fp.y() * scfg->pixelPerMMX() / scfg->contraction_ratio << " "; // doesn't need 100 actually

        // just fit to origin
        //outfile << std::fixed << (float)point.X/Hix::Polyclipping::INT_PT_RESOLUTION - scfg->origin.x() << "," << std::fixed << (float)point.Y/Hix::Polyclipping::INT_PT_RESOLUTION - scfg->origin.y() << " ";
    }
    content << "\" style=\"fill: white\" />\n";
}

void SVGexporterPrivate:: writeGroupHeader(int layer_idx, float z, std::stringstream& content){
    content << "    <g id=\"layer" << layer_idx << "\" contour:z=\""<< std::fixed << z << "\">\n";
}

void SVGexporterPrivate:: writeGroupFooter(std::stringstream& content){
    content << "    </g>\n";
}

void SVGexporterPrivate::writeHeader(std::stringstream& content){
    content << "<svg width='" << scfg->resolutionX() << "' height='" << scfg->resolutionY() << "' xmlns='http://www.w3.org/2000/svg' xmlns:contour='http://hix.co.kr' style='background-color: black;'>\n";
}

void SVGexporterPrivate::writeFooter(std::stringstream& content){
    content << "</svg>";
}

