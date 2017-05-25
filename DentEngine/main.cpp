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
    // for debug
    QApplication a(argc, argv);
    QLabel l;
    QPicture pi;
    QPainter p(&pi);

    Configuration* cfg = new Configuration();

    // Load mesh
    Mesh* loaded_mesh = new Mesh(cfg);
    //loadMeshSTL(loaded_mesh, "C:\\Users\\Diri\\Desktop\\DLP\\test\\Hollow_Draudi_small.STL");
    loadMeshSTL(loaded_mesh, "C:\\Users\\diridiri\\Desktop\\DLP\\lowerjaw.STL");
    printf("vertices : %d, faces : %d\n", loaded_mesh->vertices.size(), loaded_mesh->faces.size());
    printf("slicing in %s mode, resolution %d\n", cfg->slicing_mode, cfg->resolution);

    // Slice
    Slicer* slicer = new Slicer(cfg);
    Paths pathList = slicer->meshSlice(loaded_mesh)[359];

    Paths contourList = slicer->contourConstruct(pathList);



    Clipper clpr;
    ClipperOffset co;
    PolyTree polytree;

    clpr.AddPaths(contourList, ptSubject, true);
    clpr.Execute(ctUnion, polytree);
    qDebug() << polytree.ChildCount();
    Paths newcontourList;
    ClosedPathsFromPolyTree(polytree, newcontourList);
    co.AddPaths(newcontourList, jtRound, etClosedPolygon);
    co.Execute(newcontourList, -(cfg->wall_thickness+cfg->nozzle_width)/2);
    //PolyTreeToPaths(polytree, newcontourList);
    qDebug() << newcontourList.size() << newcontourList[0].size();
    //slicer->slice(loaded_mesh);

    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(Qt::black, 1, Qt::DashDotLine, Qt::RoundCap));
    for(Path path: newcontourList){
        //qDebug() <<path[0].X<< path[0].Y<< path[1].X<< path[1].Y;
        //p.drawLine(path[0].X/100, path[0].Y/100, path[1].X/100, path[1].Y/100);
        for (IntPoint ip : path){
         p.drawPoint(ip.X/100+100, ip.Y/100+100);
        }

    }
    qDebug() << "contour count : " << contourList.size();
    qDebug() << "----------------------";
    for(Path contour: contourList){
        qDebug() << "contour length " << contour.size();
        for (IntPoint ip : contour){
            //qDebug() << ip.X << ip.Y;
            p.drawPoint(ip.X/100 + 500,ip.Y/100 + 500);
            //p.drawLine(ip.X/100+200, ip.Y/100, contour[1].X/100 + 200, contour[1].Y/100);
        }
    }
    p.end();

    l.setPicture(pi);
    l.show();

    //*/

    /*QCoreApplication a(argc, argv);
    QCoreApplication::setApplicationName("DLPengine");
    QCoreApplication::setApplicationVersion("1.0");
    QLabel l;
    QPicture pi;
    QPainter p(&pi);

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
    */
    return a.exec();
}
