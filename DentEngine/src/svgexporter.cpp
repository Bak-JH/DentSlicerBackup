#include "svgexporter.h"

void SVGexporter::exportSVG(Slices contourLists, QString outfoldername){
    QDir dir(outfoldername);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

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
        outfile << std::fixed << (float)point.X/cfg->resolution - cfg->origin.x() << "," << std::fixed << (float)point.Y/cfg->resolution - cfg->origin.y() << " ";
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
    outfile << "<svg width='1024' height='600' xmlns='http://www.w3.org/2000/svg' xmlns:contour='http://hix.co.kr' style='background-color: black;'>\n";
}

void SVGexporter::writeFooter(ofstream& outfile){
    outfile << "</svg>";
}

