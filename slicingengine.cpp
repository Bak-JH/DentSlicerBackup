#include "slicingengine.h"
#include <QDir>

SlicingEngine::SlicingEngine()
{
}

QProcess *slicing_process;

// collect configuration and start slicing process
void SlicingEngine::slice (QVariantMap config){

    /*for(QVariantMap::const_iterator iter = config.begin(); iter != config.end(); ++iter) {
      char buf[100];
        sprintf(buf, "-%s", iter.key());
        (*cfg)[buf] = iter.value().toString();
    }*/

    QObject *parent;
    QString program = "\"C:\\Users\\diridiri\\Desktop\\DLP\\DLPbuild\\debug\\DentStudioEngine.exe\"";
    QStringList command_list;
    command_list << "C:/Users/diridiri/Desktop/DLP/partial1.STL" << "C:/Users/diridiri/Desktop/DLP/output";
    for(QVariantMap::const_iterator iter = (*cfg).begin(); iter != (*cfg).end(); ++iter) {
      QStringList valuestring = iter.value().toString().split("#");
      command_list << "-"+valuestring[0] << valuestring[1];
    }
    slicing_process = new QProcess(this);
    connect(slicing_process, SIGNAL(started()), this, SLOT(slicingStarted()));
    connect(slicing_process, SIGNAL(readyReadStandardOutput()), this, SLOT(slicingOutput()));
    connect(slicing_process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(slicingFinished(int, QProcess::ExitStatus)));
    connect(slicing_process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(slicingError(QProcess::ProcessError)));
    slicing_process->start(program, command_list);
}


void SlicingEngine::slicingStarted(){
    qDebug() << "slicing started";

}

void SlicingEngine::slicingOutput(){
    QByteArray newData = slicing_process->readAllStandardOutput();
    QString text = QString::fromLocal8Bit(newData);
    qDebug() << text;
    if (text.contains("info")){
        QStringList infos = text.trimmed().split(":");
        int time = infos[1].toInt();
        int layer = infos[2].toInt();
        QString size;
        float x = infos[3].toFloat(), y=infos[4].toFloat(), z=infos[5].toFloat(); // x = text.trimmed().split(":")
        size.sprintf("%.1f X %.1f X %.1f mm",x,y,z);
        float volume = infos[6].toFloat();

        emit updateModelInfo(time, layer, size, volume);
    }
    if (text.contains("done")){ // progressbar update
        if (text.contains("slicing")){
            //popup slicing done
        }
    }
}

void SlicingEngine::slicingFinished(int finish_status, QProcess::ExitStatus pes){
    qDebug() << "slicing finished" << finish_status << pes;
}

void SlicingEngine::slicingError(QProcess::ProcessError pe){
    qDebug()<< "slicinig error" << pe;
}

