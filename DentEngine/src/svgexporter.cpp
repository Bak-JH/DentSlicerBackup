#include "svgexporter.h"

void SVGexporter::exportSVG(Slices contourLists, const char* outfilename){
    ofstream outfile(outfilename, ios::out);

    writeHeader(outfile);

    for (int i=0; i<contourLists.size(); i++){
        if (cfg->slicing_mode == "uniform")
            writeGroupHeader(outfile, i, cfg->layer_height*(i+1));
        else
            writeGroupHeader(outfile, i, cfg->layer_height*(i+1));
        for (Path contour : contourLists[i]){
            writePolygon(outfile, contour);
        }
        writeGroupFooter(outfile);
    }

    writeFooter(outfile);

    outfile.close();
    printf("done exporting\n");
}

void SVGexporter::writePolygon(ofstream& outfile, Path contour){
    outfile << "      <polygon DLPengine:type=\"DLPengine\" points=\"";
    for (IntPoint point: contour){
        outfile << std::fixed << (float)point.X/cfg->resolution << "," << std::fixed << (float)point.Y/cfg->resolution << " ";
    }
    outfile << "\" style=\"fill: white\" />\n";

}

void SVGexporter:: writeGroupHeader(ofstream& outfile, int layer_idx, float z){
    outfile << "    <g id=\"layer" << layer_idx << "\" DLPengine:z=\""<< std::fixed << z << "\">\n";
}

void SVGexporter:: writeGroupFooter(ofstream& outfile){
    outfile << "    </g>\n";
}

void SVGexporter::writeHeader(ofstream& outfile){
    outfile << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>";
    outfile << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.0//EN\" \"http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd\">";
    outfile << "<svg width=\"13\" height=\"13\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:svg=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" xmlns:diridiri=\"http://hix.co.kr\">\n";
    outfile << "    <!-- Generated using DLPengine 1.0.0\n    http://hix.co.kr/\n    -->\n";
}

void SVGexporter::writeFooter(ofstream& outfile){
    outfile << "</svg>";
}

