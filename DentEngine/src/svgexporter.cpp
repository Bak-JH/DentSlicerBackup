#include "svgexporter.h"
#include "slicer.h"
#include "polyclipping/clipper/clipper.hpp"
using namespace ClipperLib;

int origin_x;
int origin_y;
int origin_z;

#if defined(_DEBUG) || defined(QT_DEBUG)
#define _DEBUG_SVG
#endif


#ifdef _DEBUG_SVG
using namespace Hix::Debug;
void logSlices(const Slices& slices)
{
	qDebug() << slices;

}
#endif
namespace SVGexporterPrivate
{
	void parsePolyTreeAndWrite(ClipperLib::PolyNode* pn, std::ofstream& outfile);
	void writePolygon(std::ofstream& outfile, ClipperLib::Path& contour);
	void writePolygon(std::ofstream& outfile, ClipperLib::PolyNode* contour);
	void writeGroupHeader(std::ofstream& outfile, int layer_idx, float z);
	void writeGroupFooter(std::ofstream& outfile);
	void writeHeader(std::ofstream& outfile);
	void writeFooter(std::ofstream& outfile);
}


QString SVGexporter::exportSVG(Slices& shellSlices, Slices& supportSlices, Slices& raftSlices, QString outfoldername){
	using namespace SVGexporterPrivate;

#ifdef _DEBUG_SVG
	qDebug() << "logging slices";
	qDebug() << "shellSlices entireClass : ";
	logSlices(shellSlices);
	qDebug() << "supportSlices entireClass : ";
	logSlices(supportSlices);
	qDebug() << "raftSlices entireClass : ";
	logSlices(raftSlices);
#endif



    qDebug() << "export svg at "<< outfoldername;
    qDebug() << "shellSlices : " << shellSlices.size() << "supportSlices : " << supportSlices.size();
    QDir dir(outfoldername);
    if (!dir.exists()) {
        dir.mkpath(".");
    } else {
        dir.removeRecursively();
        dir.mkpath(".");
    }

    QString infofilename = outfoldername + "/" + "info.json";
    QFile infofile(infofilename);
    infofile.open(QFile::WriteOnly);

    //std::ofstream infofile(infofilename.toStdString().c_str(), std::ios::out);
    QJsonObject jsonObject;
    jsonObject["layer_height"] = round(scfg->layer_height*100)/100;
    jsonObject["total_layer"] = int(shellSlices.size() + round(scfg->support_base_height/scfg->layer_height) + raftSlices.size());
    jsonObject["bed_curing_time"] = 15000; // depends on scfg->resin_type
    jsonObject["curing_time"] = 2100; // depends on scfg->resin_type
    jsonObject["mirror_rot_time"] = 2000;
    jsonObject["z_hop_height"] = 15;
    jsonObject["move_up_feedrate"] = 350;
    jsonObject["move_down_feedrate"] = 500;
    jsonObject["resin_type"] = scfg->resin_type;
    jsonObject["contraction_ratio"] = scfg->contraction_ratio;
    QJsonDocument jsonDocument(jsonObject);
    QByteArray jsonBytes = jsonDocument.toJson();
    infofile.write(jsonBytes);
    infofile.close();
    //qDebug() << jsonBytes;

    origin_x = scfg->origin.x()*scfg->resolution;
    origin_y = scfg->origin.y()*scfg->resolution;
    origin_z = scfg->origin.z()*scfg->resolution;

    int64_t area = 0;
    int currentSlice_idx = 0;

    // to prevent empty raftSlice[0]
    if (raftSlices.size() != 0 && raftSlices[0].size() == 0){
        raftSlices[0] = raftSlices[1];
    }

    // to prevent empty supportSlice[0]
    if (supportSlices.size() != 0 && supportSlices[0].size() == 0){
        supportSlices[0] = supportSlices[1];
    }

    // to prevent empty shellSlice[0]
    if (shellSlices.size() != 0 && shellSlices[0].size() == 0){
        shellSlices[0] = shellSlices[1];
    }

    for (int i=0; i<raftSlices.size(); i++){
        QString outfilename = outfoldername + "/" + QString::number(currentSlice_idx) + ".svg";

        std::ofstream outfile(outfilename.toStdString().c_str(), std::ios::out);

        writeHeader(outfile);
        if (scfg->slicing_mode == "uniform")
            writeGroupHeader(outfile, currentSlice_idx, scfg->layer_height*(currentSlice_idx+1));
        else
            writeGroupHeader(outfile, currentSlice_idx, scfg->layer_height*(currentSlice_idx+1));

        for (int j=0; j<raftSlices[i].outershell.size(); j++){
            writePolygon(outfile, raftSlices[i].outershell[j]);
        }

        writeGroupFooter(outfile);
        writeFooter(outfile);

        outfile.close();

        currentSlice_idx += 1;
    }

    qDebug() << "Raft Slices : " << currentSlice_idx;

    int support_base_layer_cnt = round(scfg->support_base_height/scfg->layer_height);

    if (supportSlices.size() != 0){ // generate support base only when support is being generated

        // add base support layers
        for (int i=0; i<support_base_layer_cnt; i++){
            QString outfilename = outfoldername + "/" + QString::number(currentSlice_idx) + ".svg";

            std::ofstream outfile(outfilename.toStdString().c_str(), std::ios::out);

            writeHeader(outfile);

            if (scfg->slicing_mode == "uniform")
                writeGroupHeader(outfile, currentSlice_idx, scfg->layer_height*(currentSlice_idx+1));
            else
                writeGroupHeader(outfile, currentSlice_idx, scfg->layer_height*(currentSlice_idx+1));

            // write support slices
            for (int j=0; j<supportSlices[i].outershell.size(); j++){
                writePolygon(outfile, supportSlices[i].outershell[j]);
            }

            writeGroupFooter(outfile);
            writeFooter(outfile);

            outfile.close();
            currentSlice_idx += 1;
        }

    }

    for (int i=0; i<shellSlices.size(); i++){
        QString outfilename = outfoldername + "/" + QString::number(currentSlice_idx) + ".svg";

        std::ofstream outfile(outfilename.toStdString().c_str(), std::ios::out);

        writeHeader(outfile);

        if (scfg->slicing_mode == "uniform")
            writeGroupHeader(outfile, currentSlice_idx, scfg->layer_height*(currentSlice_idx+1));
        else
            writeGroupHeader(outfile, currentSlice_idx, scfg->layer_height*(currentSlice_idx+1));

        PolyTree shellSlice_polytree = shellSlices[i].polytree;
        //qDebug() << "slice polytree's child count : " << shellSlice_polytree.ChildCount();
        for (int j=0; j<shellSlice_polytree.ChildCount(); j++){
            parsePolyTreeAndWrite(shellSlice_polytree.Childs[j], outfile);
        }

        qDebug() << "support slicessize : " << supportSlices.size() << int(supportSlices.size()) - support_base_layer_cnt << (supportSlices.size() - support_base_layer_cnt > i);
        // write support slices
        if (int(supportSlices.size())-support_base_layer_cnt > i){

            for (int j=0; j<supportSlices[i+support_base_layer_cnt].outershell.size(); j++){
                writePolygon(outfile, supportSlices[i+support_base_layer_cnt].outershell[j]);
            }
        }

        writeGroupFooter(outfile);
        writeFooter(outfile);

        outfile.close();
        currentSlice_idx += 1;
    }

    //printf("slicing done\n");
    int layer = shellSlices.size();
    int printing_time = layer*15/60;

    float x = shellSlices.mesh->x_max()-shellSlices.mesh->x_min();
    float y = shellSlices.mesh->y_max()-shellSlices.mesh->y_min();
    float z = shellSlices.mesh->z_max()-shellSlices.mesh->z_min();

    float volume = ((float)(area/pow(scfg->pixel_per_mm/scfg->contraction_ratio,2))/1000000)*scfg->layer_height;
    QString result_str;
    result_str.sprintf("info:%d:%d:%.1f:%.1f:%.1f:%.1f\n",printing_time,layer,x,y,z,volume);
    //fflush(stdout);
    qDebug() << "slicing done";
    //exit(0);



    // do run svg 2 png

    int max_slices = currentSlice_idx;
    for (int i=0; i<max_slices; i++){
        QString svgfilename = outfoldername + "/" + QString::number(i) + ".svg";
        QSvgRenderer renderer(svgfilename);
        QImage image(scfg->resolution_x, scfg->resolution_y, QImage::Format_RGB32);
        image.fill(0x000000);
        QPainter painter(&image);
        renderer.render(&painter);
        image.save(outfoldername + "/S" + QString::number(i).rightJustified(6,'0') + "_P1.png");

    }

    return result_str;
}




void SVGexporterPrivate::parsePolyTreeAndWrite(PolyNode* pn, std::ofstream& outfile){
    writePolygon(outfile, pn);
    for (int i=0; i<pn->ChildCount(); i++){
        PolyNode* new_pn = pn->Childs[i];
        parsePolyTreeAndWrite(new_pn, outfile);
    }

    /*while (pn != NULL){
        //area += Area(pn->Contour);

        pn = pn->GetNext();
    }*/

}

void SVGexporterPrivate::writePolygon(std::ofstream& outfile, PolyNode* contour){
    outfile << "      <polygon contour:type=\"contour\" points=\"";
    for (IntPoint point: contour->Contour){
        outfile << std::fixed << (float)(point.X)*scfg->pixel_per_mm/(scfg->resolution*scfg->contraction_ratio) + (scfg->resolution_x/2)<< "," << std::fixed << scfg->resolution_y/2 - (float)(point.Y)*scfg->pixel_per_mm/(scfg->resolution*scfg->contraction_ratio) << " "; // doesn't need 100 actually

        // just fit to origin
        //outfile << std::fixed << (float)point.X/scfg->resolution - scfg->origin.x() << "," << std::fixed << (float)point.Y/scfg->resolution - scfg->origin.y() << " ";
    }
    if (! contour->IsHole()){
        outfile << "\" style=\"fill: white\" />\n";
    } else {
        outfile << "\" style=\"fill: black\" />\n";
    }
}

void SVGexporterPrivate::writePolygon(std::ofstream& outfile, ClipperLib::Path& contour){
    outfile << "      <polygon contour:type=\"contour\" points=\"";
    for (IntPoint point: contour){
        outfile << std::fixed << (float)(point.X)*scfg->pixel_per_mm/(scfg->resolution*scfg->contraction_ratio) + (scfg->resolution_x/2) << "," << std::fixed << scfg->resolution_y/2 - (float)(point.Y)*scfg->pixel_per_mm/(scfg->resolution*scfg->contraction_ratio)<< " "; // doesn't need 100 actually

        // just fit to origin
        //outfile << std::fixed << (float)point.X/scfg->resolution - scfg->origin.x() << "," << std::fixed << (float)point.Y/scfg->resolution - scfg->origin.y() << " ";
    }
    outfile << "\" style=\"fill: white\" />\n";
}

void SVGexporterPrivate:: writeGroupHeader(std::ofstream& outfile, int layer_idx, float z){
    outfile << "    <g id=\"layer" << layer_idx << "\" contour:z=\""<< std::fixed << z << "\">\n";
}

void SVGexporterPrivate:: writeGroupFooter(std::ofstream& outfile){
    outfile << "    </g>\n";
}

void SVGexporterPrivate::writeHeader(std::ofstream& outfile){
    outfile << "<svg width='" << scfg->resolution_x << "' height='" << scfg->resolution_y << "' xmlns='http://www.w3.org/2000/svg' xmlns:contour='http://hix.co.kr' style='background-color: black;'>\n";
}

void SVGexporterPrivate::writeFooter(std::ofstream& outfile){
    outfile << "</svg>";
}

