//#include "slicingengine.h"
#include <QDir>
#include "qmlmanager.h"
#include "glmodel.h"
#include "DentEngine/src/configuration.h"

SlicingEngine::SlicingEngine()
{
}

QProcess *slicing_process;

Slicer* SlicingEngine::slice( Mesh* shellMesh, Mesh* supportMesh, Mesh* raftMesh, QString filename){
    qDebug() << "slice" << shellMesh << filename;

    qDebug() << "done parsing arguments";

    qmlManager->setProgress(0.1);

    // configuration
    scfg->origin = QVector3D((shellMesh->x_min()+shellMesh->x_max())/2, (shellMesh->y_min()+shellMesh->y_max())/2, (shellMesh->z_min()+shellMesh->z_max())/2);
    // Slice
    Slicer* slicer = new Slicer();
    Slices shellSlices;
    qDebug() << "shell Mesh : " << shellMesh << shellSlices.mesh;
    slicer->slice(shellMesh, &shellSlices);
    qDebug() << "Shell Slicing Done\n";
    qmlManager->setProgress(0.4);
    Slices supportSlices;
	if (scfg->support_type != SlicingConfiguration::SupportType::None)
	{
		slicer->slice(supportMesh, &supportSlices);
		qDebug() << "Support Slicing Done\n";
	}
    qmlManager->setProgress(0.6);
    Slices raftSlices;
	if (scfg->raft_type != SlicingConfiguration::RaftType::None)
	{
		slicer->slice(raftMesh, &raftSlices);
		qDebug() << "Raft Slicing Done\n";
	}
    qmlManager->setProgress(0.9);


    // Export to SVG
    QString export_info = SVGexporter::exportSVG(shellSlices, supportSlices, raftSlices, filename+"_export");

    // 승환 100%
    qmlManager->setProgress(1);
    QStringList name_word = filename.split("/");

    QStringList infos = export_info.trimmed().split(":");
    int time = infos[1].toInt();
    int layer = infos[2].toInt();
    QString size;
    float x = infos[3].toFloat(), y=infos[4].toFloat(), z=infos[5].toFloat(); // x = text.trimmed().split(":")
    size.sprintf("%.1f X %.1f X %.1f mm",x,y,z);
    float volume = infos[6].toFloat();

    emit updateModelInfo(time, layer, size, volume);

    qmlManager->openResultPopUp("",
                                QString(name_word[name_word.size()-1]+" slicing done.").toStdString(),
                                "");

    slicer->slicingInfo = export_info;

    return slicer;
}

/*
// collect configuration and start slicing process
void SlicingEngine::slice (QString config, QString filename){

    qDebug() << "current directory : " << QDir::currentPath()+"/DentEngine/DentStudioEngine.exe";
    QObject *parent;
    QString program ="\""+QDir::currentPath()+"/DentEngine/DentStudioEngine.exe\"";
    program = program.replace("/","\\");
    qDebug() << program;
    QStringList command_list;
    command_list << filename << filename+"_temp";
    //command_list << "C:/Users/diridiri/Desktop/DLP/partial1.STL" << "C:/Users/diridiri/Desktop/DLP/output";
    QStringList configs = config.split("/");
    for (QString config : configs){
        QStringList valuestring = config.split("#");
        command_list << "-"+valuestring[0] << valuestring[1];
    }

    qDebug() << command_list;
    slicing_process = new QProcess(this);
    connect(slicing_process, SIGNAL(started()), this, SLOT(slicingStarted()));
    connect(slicing_process, SIGNAL(readyReadStandardOutput()), this, SLOT(slicingOutput()));
    connect(slicing_process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(slicingFinished(int, QProcess::ExitStatus)));
    connect(slicing_process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(slicingError(QProcess::ProcessError)));
    slicing_process->start(program, command_list);
}
*/


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

