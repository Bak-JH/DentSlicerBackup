#include <QCoreApplication>
#include "src/polyclipping/clipper.hpp"
#include "src/cmdlineparser.h"

using namespace ClipperLib;

Paths subj;
Paths clip;
Paths solution;

void addPoint(int x, int y, Path *path)
{
    IntPoint ip;
    ip.X = x;
    ip.Y = y;
    path->push_back(ip);
}

/*int main()
{

    Paths subj;

    Path p;
    addPoint(100,100, &p);
    addPoint(200,100, &p);
    addPoint(200,200, &p);
    addPoint(100,200, &p);
    subj.push_back(p);

    Path p2;
    addPoint(150,50, &p2);
    addPoint(175,50, &p2);
    addPoint(175,250, &p2);
    addPoint(150,250, &p2);
    clip.push_back(p2);

    Clipper c;

    c.AddPaths(subj, ptSubject, true);
    c.AddPaths(clip, ptClip, true);
    c.Execute(ctIntersection, solution, pftNonZero, pftNonZero);

    printf("solution size = %d\n",(int)solution.size());
    for (unsigned i=0; i<solution.size(); i++)
    {
        Path p3 = solution.at(i);

        for (unsigned j=0; j<p3.size(); j++)
        {
            IntPoint ip = p3.at(j);
            printf("%d = %lld, %lld\n",j, ip.X,ip.Y);
        }

    }

    return 0;
}*/

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QCoreApplication::setApplicationName("DLPengine");
    QCoreApplication::setApplicationVersion("1.0");

    CmdLineParser parser;
    QString errorMsg;

    switch (parser.parseCommandLine(&errorMsg)) {
        case CommandLineOk:
            break;
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

    return a.exec();
}
