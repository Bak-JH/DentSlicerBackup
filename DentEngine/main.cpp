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

    Configuration* cfg = new Configuration();

    // Load mesh
    Mesh* loaded_mesh = new Mesh(cfg);
    //loadMeshSTL(loaded_mesh, "C:\\Users\\Diri\\Desktop\\DLP\\test\\Hollow_Draudi_small.STL");
    loadMeshSTL(loaded_mesh, "C:\\Users\\diridiri\\Desktop\\DLP\\lowerjaw.STL");
    printf("vertices : %d, faces : %d\n", loaded_mesh->vertices.size(), loaded_mesh->faces.size());
    printf("slicing in %s mode, resolution %d\n", cfg->slicing_mode, cfg->resolution);
    printf("debugging layer : %d\n", debug_layer);
    // Slice
    Slicer* slicer = new Slicer(cfg);

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
    Paths contourList = slices[0].overhang_region;
//    Paths contourList = slices[2].outershell;
//    Paths contourList = slices[debug_layer].outershell;

    QLabel l;
    l.setGeometry(0,0,1000,600);
    QPicture pi;
    QPainter p(&pi);

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

    // draw overhang positions
    qDebug() << slices.overhang_positions.size();
    p.setPen(QPen(Qt::red, 2, Qt::DashLine, Qt::RoundCap));
    for (QVector3D cop : slices.overhang_positions){
        qDebug() << "pointing " << cop.x() << cop.y();
        p.drawPoint(cop.x()*10 + 500, cop.y()*10 + 500);
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
            // Parse configuration
            Configuration* cfg = new Configuration();

            // Load mesh
            Mesh* loaded_mesh = new Mesh(cfg);
            loadMeshSTL(loaded_mesh, parser.inputfilename.toStdString().c_str());
            printf("vertices : %d, faces : %d\n", loaded_mesh->vertices.size(), loaded_mesh->faces.size());
            printf("slicing in %s mode, resolution %d\n", cfg->slicing_mode, cfg->resolution);

            // Slice
            Slicer* slicer = new Slicer(cfg);
            Slices contourLists = slicer->slice(loaded_mesh);

            // Export to SVG
            SVGexporter* exporter = new SVGexporter(cfg);
            //exporter->exportSVG(contourLists, parser.outputfilename.toStdString().c_str());

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
