#include "svgexporter.h"

int origin_x;
int origin_y;
int origin_z;

void SVGexporter::exportSVG(Slices contourLists, QString outfoldername){
    qDebug() << "export svg at "<< outfoldername;
    QDir dir(outfoldername);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    QString infofilename = outfoldername + "/" + "info.json";
    ofstream infofile(infofilename.toStdString().c_str(), ios::out);
    QJsonObject jsonObject;
    jsonObject["layer_height"] = round(scfg->layer_height*100)/100;
    jsonObject["total_layer"] = int(contourLists.size());
    jsonObject["bed_curing_time"] = 15000; // depends on scfg->resin_type
    jsonObject["curing_time"] = 2100; // depends on scfg->resin_type
    jsonObject["mirror_rot_time"] = 2000;
    jsonObject["z_hop_height"] = 15;
    QJsonDocument jsonDocument(jsonObject);
    QByteArray jsonBytes = jsonDocument.toJson();

    infofile << jsonBytes.toStdString();
    infofile.close();
    //qDebug() << jsonBytes;

    origin_x = scfg->origin.x()*scfg->resolution;
    origin_y = scfg->origin.y()*scfg->resolution;
    origin_z = scfg->origin.z()*scfg->resolution;

    int64_t area = 0;

    for (int i=0; i<contourLists.size(); i++){
        QString outfilename = outfoldername + "/" + QString::number(i) + ".svg";

        ofstream outfile(outfilename.toStdString().c_str(), ios::out);

        writeHeader(outfile);
        if (scfg->slicing_mode == "uniform")
            writeGroupHeader(outfile, i, scfg->layer_height*(i+1));
        else
            writeGroupHeader(outfile, i, scfg->layer_height*(i+1));

        PolyTree slice_polytree = contourLists[i].polytree;
        PolyNode* pn = slice_polytree.GetFirst();
        while (pn != NULL){
            area += Area(pn->Contour);
            writePolygon(outfile, pn);
            pn = pn->GetNext();
        }

        writeGroupFooter(outfile);
        writeFooter(outfile);

        outfile.close();
    }
    //printf("slicing done\n");
    int layer = contourLists.size();
    int printing_time = layer*15/60;

    float x = contourLists.mesh->x_max-contourLists.mesh->x_min;
    float y = contourLists.mesh->y_max-contourLists.mesh->y_min;
    float z = contourLists.mesh->z_max-contourLists.mesh->z_min;

    float volume = ((float)(area/pow(scfg->pixel_per_mm,2))/1000000)*scfg->layer_height;
    //printf("info:%d:%d:%.1f:%.1f:%.1f:%.1f\n",printing_time,layer,x,y,z,volume);
    //fflush(stdout);
    qDebug() << "slicing done";
    //exit(0);
    return;
}

void SVGexporter::writePolygon(ofstream& outfile, PolyNode* contour){
    outfile << "      <polygon contour:type=\"contour\" points=\"";
    for (IntPoint point: contour->Contour){
        outfile << std::fixed << (float)(point.X-origin_x)*scfg->pixel_per_mm/scfg->resolution + scfg->resolution_x/2 << "," << std::fixed << (float)(point.Y-origin_y)*scfg->pixel_per_mm/scfg->resolution + scfg->resolution_y/2 << " "; // doesn't need 100 actually

        // just fit to origin
        //outfile << std::fixed << (float)point.X/scfg->resolution - scfg->origin.x() << "," << std::fixed << (float)point.Y/scfg->resolution - scfg->origin.y() << " ";
    }
    if (! contour->IsHole()){
        outfile << "\" style=\"fill: white\" />\n";
    } else {
        outfile << "\" style=\"fill: black\" />\n";
    }

}

void SVGexporter::writePolygon(ofstream& outfile, Path contour){
    outfile << "      <polygon contour:type=\"contour\" points=\"";
    for (IntPoint point: contour){
        outfile << std::fixed << (float)(point.X-origin_x)*scfg->pixel_per_mm/scfg->resolution + scfg->resolution_x/2 << "," << std::fixed << (float)(point.Y-origin_y)*scfg->pixel_per_mm/scfg->resolution + scfg->resolution_y/2 << " "; // doesn't need 100 actually

        // just fit to origin
        //outfile << std::fixed << (float)point.X/scfg->resolution - scfg->origin.x() << "," << std::fixed << (float)point.Y/scfg->resolution - scfg->origin.y() << " ";
    }
    if (Orientation(contour)){
        outfile << "\" style=\"fill: white\" />\n";
    } else {
        outfile << "\" style=\"fill: black\" />\n";
    }

}

void SVGexporter:: writeGroupHeader(ofstream& outfile, int layer_idx, float z){
    outfile << "    <g id=\"layer" << layer_idx << "\" contour:z=\""<< std::fixed << z << "\">\n";
}

void SVGexporter:: writeGroupFooter(ofstream& outfile){
    outfile << "    </g>\n";
}

void SVGexporter::writeHeader(ofstream& outfile){
    outfile << "<svg width='" << scfg->resolution_x << "' height='" << scfg->resolution_y << "' xmlns='http://www.w3.org/2000/svg' xmlns:contour='http://hix.co.kr' style='background-color: black;'>\n";
}

void SVGexporter::writeFooter(ofstream& outfile){
    outfile << "</svg>";
}

