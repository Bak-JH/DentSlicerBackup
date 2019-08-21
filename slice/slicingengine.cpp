//#include "slicingengine.h"
#include <QDir>
#include <QObject>
#include <QDebug>
#include <QProcess>
#include "feature/stlexporter.h"
#include "DentEngine/src/svgexporter.h"
#include "slicingengine.h"
#include "qmlmanager.h"
#include "glmodel.h"
#include "DentEngine/src/configuration.h"
#include "DentEngine/src/slicer.h"
#include "DentEngine/src/Planes.h"
#include "feature/overhangDetect.h"
using namespace Hix;
using namespace Hix::Slicer;
SlicingEngine::Result SlicingEngine::sliceModel(bool isTemp, tf::Subflow& subflow, Mesh* shellMesh, Mesh* supportMesh, Mesh* raftMesh, QString filename){
    qDebug() << "slice" << shellMesh << filename;

    qDebug() << "done parsing arguments";
	auto ptOverhangs = OverhangDetect::detectOverhang(shellMesh);

    qmlManager->setProgress(0.1);

	//generate planes
	//if (scfg->slicing_mode == SlicingConfiguration::SlicingMode::Uniform) {
	float delta = scfg->layer_height;
	UniformPlanes planes(shellMesh, delta);
	planes.buildTriangleLists();
    // Slice
    Slices shellSlices;
    qDebug() << "shell Mesh : " << shellMesh << shellSlices.mesh;
    Slicer::slice(shellMesh, &planes, &shellSlices);
    qDebug() << "Shell Slicing Done\n";
    qmlManager->setProgress(0.4);

	//support

 // 
	//Slices supportSlices;
	//if (scfg->support_type != SlicingConfiguration::SupportType::None)
	//{
	//	Slicer::slice(supportMesh, &supportSlices);
	//	qDebug() << "Support Slicing Done\n";
	//}
 //   qmlManager->setProgress(0.6);
 //   Slices raftSlices;
	//if (scfg->raft_type != SlicingConfiguration::RaftType::None)
	//{
	//	Slicer::slice(raftMesh, &raftSlices);
	//	qDebug() << "Raft Slicing Done\n";
	//}
 //   qmlManager->setProgress(0.9);


    // Export to SVG
    //QString export_info = SVGexporter::exportSVG(shellSlices, supportSlices, raftSlices, filename+"_export", isTemp);
	Slices emptySupp;

	QString export_info = SVGexporter::exportSVG(shellSlices, emptySupp, emptySupp, filename + "_export", isTemp);

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
    qmlManager->openResultPopUp("",
                                QString(name_word[name_word.size()-1]+" slicing done.").toStdString(),
                                "");
	return { time , layer, size, volume };
}