#include "slicingengine.h"
#include <QDir>

SlicingEngine::SlicingEngine()
{
}


// collect configuration and start slicing process
void SlicingEngine::slice (QVariantMap config){

    for(QVariantMap::const_iterator iter = config.begin(); iter != config.end(); ++iter) {
      (*cfg)[iter.key()] = iter.value().toString();
    }

    for(QVariantMap::const_iterator iter = (*cfg).begin(); iter != (*cfg).end(); ++iter) {
      qDebug() << iter.key() << iter.value();
    }


    QObject *parent;
    QString program = "D:/Dev/DLPSlicer/build-DLPslicer-Desktop_Qt_5_8_0_MinGW_32bit-Debug/DentStudioEngine/DentStudioEngine.exe";
    qDebug() << QDir::currentPath();
    QStringList arguments;
    //arguments << (*cfg)["inputfilepath"].toString() << (*cfg)["outputfilepath"].toString();
    //arguments << "C:/Users/diridiri/Desktop/DLP/lowerjaw.STL" << "C:/Users/diridiri/Desktop/DLP/output";

    QProcess *slicing_process = new QProcess(parent);
    qDebug() << slicing_process->startDetached(program, arguments);
    //slicing_process->waitForFinished();
}
