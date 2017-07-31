#include <QCoreApplication>
#include "src/cmdlineparser.h"
#include "src/fileloader.h"
#include "src/configuration.h"
#include "src/slicer.h"
#include "src/svgexporter.h"
#include <QDebug>
#include <QPicture>
#include <QPainter>
#include <QApplication>
#include <QLabel>

int main(int argc, char *argv[])
{

#if debug_mode == 1

    // for debug
    QApplication a(argc, argv);

    // Load mesh
    Mesh* loaded_mesh = new Mesh();
    //loadMeshSTL(loaded_mesh, "C:\\Users\\Diri\\Desktop\\DLP\\test\\Hollow_Draudi_small.STL");
    loadMeshSTL(loaded_mesh, "C:\\Users\\diridiri\\Desktop\\DLP\\overhang_test_small.STL");
    printf("vertices : %d, faces : %d\n", loaded_mesh->vertices.size(), loaded_mesh->faces.size());
    printf("slicing in %s mode, resolution %d\n", cfg->slicing_mode, cfg->resolution);
    printf("debugging layer : %d\n", debug_layer);
    // Slice
    Slicer* slicer = new Slicer();

//    vector<Paths> meshslices = slicer->meshSlice(loaded_mesh);
//    Paths contourList = meshslices[debug_layer];
//    Slice meshslice;
//    meshslice.outershell = slicer->contourConstruct(meshslices[22]);
//    meshslice.outerShellOffset(-(cfg->wall_thickness+cfg->nozzle_width)/2, jtRound);
//    Paths contourList = meshslice.outershell;
//    meshslices = slicer->slice(lo)
//    Paths contourList = meshslice.overhang_region;

//    Paths contourList = slicer->slice(loaded_mesh)[debug_layer].overhang_region;
    Slices slices = slicer->slice(loaded_mesh);
//    int layer_num = round(slices.overhang_positions[5].z()/cfg->layer_height);
//    qDebug() << slices.overhang_positions[5].z() << slices.overhang_positions[5].z()/cfg->layer_height << layer_num;
//    Paths contourList = slices[debug_layer].outershell;
    Paths contourList = slices[30].overhang_region;
    Paths totalContour = slices[30].outershell;
//    Paths contourList = slices[2].outershell;
//    Paths contourList = slices[debug_layer].outershell;

    QLabel l;
    l.setGeometry(0,0,1000,600);
    QPicture pi;
    QPainter p(&pi);

    // overhang region
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(Qt::black, 2, Qt::DashDotLine, Qt::SquareCap));
    qDebug() << "contour count : " << contourList.size();
    qDebug() << "----------------------";
    for (int contour_idx=0; contour_idx < contourList.size() ; contour_idx++){
        Path contour = contourList[contour_idx];
        qDebug() << "contour length " << contour.size() << contour_idx;
        IntPoint start_ip = contour[0];
        IntPoint prev_ip = contour[0];
        for (IntPoint ip : contour){
            //qDebug() << ip.X << ip.Y;
            p.drawLine(prev_ip.X*10/Configuration::resolution + 500,prev_ip.Y*10/Configuration::resolution + 500, ip.X*10/Configuration::resolution + 500,ip.Y*10/Configuration::resolution + 500);
            //p.drawPoint(ip.X*10/Configuration::resolution + 500,ip.Y*10/Configuration::resolution + 500);
            prev_ip = ip;
        }
        p.drawLine(prev_ip.X*10/Configuration::resolution + 500,prev_ip.Y*10/Configuration::resolution + 500, start_ip.X*10/Configuration::resolution + 500,start_ip.Y*10/Configuration::resolution + 500);
    }

    // outershell
    p.setPen(QPen(Qt::green, 2, Qt::DashLine, Qt::RoundCap));
    for (int contour_idx=0; contour_idx < totalContour.size() ; contour_idx++){
        Path contour = totalContour[contour_idx];
        qDebug() << "contour length " << contour.size() << contour_idx;
        IntPoint start_ip = contour[0];
        IntPoint prev_ip = contour[0];
        for (IntPoint ip : contour){
            //qDebug() << ip.X << ip.Y;
            p.drawLine(prev_ip.X*10/Configuration::resolution + 500,prev_ip.Y*10/Configuration::resolution + 500, ip.X*10/Configuration::resolution + 500,ip.Y*10/Configuration::resolution + 500);
            //p.drawPoint(ip.X*10/Configuration::resolution + 500,ip.Y*10/Configuration::resolution + 500);
            prev_ip = ip;
        }
        p.drawLine(prev_ip.X*10/Configuration::resolution + 500,prev_ip.Y*10/Configuration::resolution + 500, start_ip.X*10/Configuration::resolution + 500,start_ip.Y*10/Configuration::resolution + 500);
    }


    // intersection points
    p.setPen(QPen(Qt::blue, 10, Qt::DashLine, Qt::RoundCap));
    for(IntPoint ip : slices.intersectionPoints){
       p.drawPoint(ip.X*10/Configuration::resolution + 500, ip.Y*10/Configuration::resolution + 500);
    }

    // test distance
    /*p.setPen(QPen(Qt::yellow, 10, Qt::DashLine, Qt::RoundCap));
    p.drawPoint(500, 500);
    p.drawPoint(500+cfg->duplication_radius, 500);*/

    /*for (int contour_idx =0; contour_idx < totalContour.size(); contour_idx ++){
        Path contour = totalContour[contour_idx];
        for (IntPoint ip: contour){
            if (ip.Z !=0) {
                qDebug() << "intersection zzzzz : ";
                p.drawPoint(ip.X*10/Configuration::resolution + 500, ip.Y*10/Configuration::resolution + 500);
            }
        }
    }*/

    // draw overhang positions
    qDebug() << slices.overhang_positions.size();
    p.setPen(QPen(Qt::red, 2, Qt::DashLine, Qt::RoundCap));
    for (OverhangPosition cop : slices.overhang_positions){
        p.drawPoint(cop.X*10/Configuration::resolution + 500, cop.Y*10/Configuration::resolution + 500);

        qDebug() << "overhang positions " << cop.X << cop.Y;
    }
    p.end();

    l.setPicture(pi);
    l.show();

#else

    QCoreApplication a(argc, argv);
    QCoreApplication::setApplicationName("DLPengine");
    QCoreApplication::setApplicationVersion("1.0");

    CmdLineParser parser;
    QString errorMsg;

    switch (parser.parseCommandLine(&errorMsg)) {
        case CommandLineOk:
        {

            // Load mesh
            Mesh* loaded_mesh = new Mesh();
            loadMeshSTL(loaded_mesh, parser.inputfilename.toStdString().c_str());
            printf("vertices : %d, faces : %d\n", loaded_mesh->vertices.size(), loaded_mesh->faces.size());
            printf("slicing in %s mode, resolution %d\n", cfg->slicing_mode, cfg->resolution);

            // Slice
            Slicer* slicer = new Slicer();
            Slices contourLists = slicer->slice(loaded_mesh);

            // Export to SVG
            qDebug() << contourLists[5].size();
            SVGexporter* exporter = new SVGexporter();
            exporter->exportSVG(contourLists, parser.outputfilename.toStdString().c_str());

            break;
        }
        case CommandLineError:
            fputs(qPrintable(errorMsg), stderr);
            fputs("\n\n", stderr);
            fputs(qPrintable(parser.helpText()), stderr);
            return 1;
        case CommandLineVersionRequested:
            printf("%s %s\n", qPrintable(QCoreApplication::applicationName()),
                   qPrintable(QCoreApplication::applicationVersion()));
            return 0;
        case CommandLineHelpRequested:
            parser.showHelp();
            Q_UNREACHABLE();
        }

#endif // debug mode selection

    return a.exec();
}
