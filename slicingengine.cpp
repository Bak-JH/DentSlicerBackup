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
    QString program = "C:/Users/diridiri/Desktop/DLP/DLPbuild/debug/DentStudioEngine.exe"; //"D:/Dev/DLPSlicer/build-DLPslicer-Desktop_Qt_5_8_0_MinGW_32bit-Debug/DentStudioEngine/DentStudioEngine.exe";
    QStringList command_list;
    command_list << program << (*cfg)["inputfilepath"].toString() << (*cfg)["outputfilepath"].toString() << "&";

    QString command = command_list.join(" ");
    system(command.toStdString().c_str());
    //QProcess *slicing_process = new QProcess(parent);
    //qDebug() << slicing_process->startDetached(program, arguments);
    //slicing_process->waitForFinished();
}
