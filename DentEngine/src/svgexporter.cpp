#include "svgexporter.h"

int origin_x;
int origin_y;
int origin_z;

void SVGexporter::exportSVG(Slices contourLists, QString outfoldername){
    QDir dir(outfoldername);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    QString infofilename = outfoldername + "/" + "info.json";
    ofstream infofile(infofilename.toStdString().c_str(), ios::out);
    QJsonObject jsonObject;
    jsonObject["layer_height"] = round(cfg->layer_height*100)/100;
    jsonObject["total_layer"] = int(contourLists.size());
    jsonObject["bed_curing_time"] = 15000; // depends on cfg->resin_type
    jsonObject["curing_time"] = 2100; // depends on cfg->resin_type
    jsonObject["mirror_rot_time"] = 2000;
    jsonObject["z_hop_height"] = 15;
    QJsonDocument jsonDocument(jsonObject);
    QByteArray jsonBytes = jsonDocument.toJson();

    infofile << jsonBytes.toStdString();
    infofile.close();
    qDebug() << jsonBytes;

    origin_x = cfg->origin.x()*cfg->resolution;
    origin_y = cfg->origin.y()*cfg->resolution;
    origin_z = cfg->origin.z()*cfg->resolution;

    for (int i=0; i<contourLists.size(); i++){
        QString outfilename = outfoldername + "/" + QString::number(i) + ".svg";

        ofstream outfile(outfilename.toStdString().c_str(), ios::out);

        writeHeader(outfile);
        if (cfg->slicing_mode == "uniform")
            writeGroupHeader(outfile, i, cfg->layer_height*(i+1));
        else
            writeGroupHeader(outfile, i, cfg->layer_height*(i+1));

        for (Path contour : contourLists[i].outershell){
            writePolygon(outfile, contour);
        }
        writeGroupFooter(outfile);
        writeFooter(outfile);

        outfile.close();
    }
    printf("done exporting\n");
}

void SVGexporter::writePolygon(ofstream& outfile, Path contour){
    outfile << "      <polygon contour:type=\"contour\" points=\"";
    for (IntPoint point: contour){
        outfile << std::fixed << (float)(point.X-origin_x)*cfg->pixel_per_mm/cfg->resolution + cfg->resolution_x/2 << "," << std::fixed << (float)(point.Y-origin_y)*cfg->pixel_per_mm/cfg->resolution + cfg->resolution_y/2 << " "; // doesn't need 100 actually

        // just fit to origin
        //outfile << std::fixed << (float)point.X/cfg->resolution - cfg->origin.x() << "," << std::fixed << (float)point.Y/cfg->resolution - cfg->origin.y() << " ";
    }
    outfile << "\" style=\"fill: white\" />\n";

}

void SVGexporter:: writeGroupHeader(ofstream& outfile, int layer_idx, float z){
    outfile << "    <g id=\"layer" << layer_idx << "\" contour:z=\""<< std::fixed << z << "\">\n";
}

void SVGexporter:: writeGroupFooter(ofstream& outfile){
    outfile << "    </g>\n";
}

void SVGexporter::writeHeader(ofstream& outfile){
    outfile << "<svg width='" << cfg->resolution_x << "' height='" << cfg->resolution_y << "' xmlns='http://www.w3.org/2000/svg' xmlns:contour='http://hix.co.kr' style='background-color: black;'>\n";
}

void SVGexporter::writeFooter(ofstream& outfile){
    outfile << "</svg>";
}

