#include "svgexporter.h"

int origin_x;
int origin_y;
int origin_z;

QString SVGexporter::exportSVG(Slices shellSlices, Slices supportSlices, Slices raftSlices, QString outfoldername){
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
    jsonObject["total_layer"] = int(shellSlices.size());
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

        // write support slices
        if (supportSlices.size()-support_base_layer_cnt > i){

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
    return result_str;
}


void SVGexporter::parsePolyTreeAndWrite(PolyNode* pn, std::ofstream& outfile){
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

void SVGexporter::writePolygon(std::ofstream& outfile, PolyNode* contour){
    outfile << "      <polygon contour:type=\"contour\" points=\"";
    for (IntPoint point: contour->Contour){
        outfile << std::fixed << (float)(2*origin_x - point.X)*scfg->pixel_per_mm/(scfg->resolution*scfg->contraction_ratio) + (scfg->resolution_x/2)<< "," << std::fixed << scfg->resolution_y/2 - (float)(2*origin_y-point.Y)*scfg->pixel_per_mm/(scfg->resolution*scfg->contraction_ratio) << " "; // doesn't need 100 actually

        // just fit to origin
        //outfile << std::fixed << (float)point.X/scfg->resolution - scfg->origin.x() << "," << std::fixed << (float)point.Y/scfg->resolution - scfg->origin.y() << " ";
    }
    if (! contour->IsHole()){
        outfile << "\" style=\"fill: white\" />\n";
    } else {
        outfile << "\" style=\"fill: black\" />\n";
    }
}

void SVGexporter::writePolygon(std::ofstream& outfile, Path contour){
    outfile << "      <polygon contour:type=\"contour\" points=\"";
    for (IntPoint point: contour){
        outfile << std::fixed << (float)(2*origin_x - point.X)*scfg->pixel_per_mm/(scfg->resolution*scfg->contraction_ratio) + (scfg->resolution_x/2) << "," << std::fixed << scfg->resolution_y/2 - (float)(2*origin_y-point.Y)*scfg->pixel_per_mm/(scfg->resolution*scfg->contraction_ratio)<< " "; // doesn't need 100 actually

        // just fit to origin
        //outfile << std::fixed << (float)point.X/scfg->resolution - scfg->origin.x() << "," << std::fixed << (float)point.Y/scfg->resolution - scfg->origin.y() << " ";
    }
    outfile << "\" style=\"fill: white\" />\n";
}

void SVGexporter:: writeGroupHeader(std::ofstream& outfile, int layer_idx, float z){
    outfile << "    <g id=\"layer" << layer_idx << "\" contour:z=\""<< std::fixed << z << "\">\n";
}

void SVGexporter:: writeGroupFooter(std::ofstream& outfile){
    outfile << "    </g>\n";
}

void SVGexporter::writeHeader(std::ofstream& outfile){
    outfile << "<svg width='" << scfg->resolution_x << "' height='" << scfg->resolution_y << "' xmlns='http://www.w3.org/2000/svg' xmlns:contour='http://hix.co.kr' style='background-color: black;'>\n";
}

void SVGexporter::writeFooter(std::ofstream& outfile){
    outfile << "</svg>";
}

