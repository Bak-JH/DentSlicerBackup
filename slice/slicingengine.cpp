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
SlicingEngine::Result SlicingEngine::sliceModels(bool isTemp, tf::Subflow& subflow, float zMax, float zMin, std::vector<const GLModel*> models, QString filename){


    qmlManager->setProgress(0.1);

	//generate planes
	//if (scfg->slicing_mode == SlicingConfiguration::SlicingMode::Uniform) {
	float delta = scfg->layer_height;
	UniformPlanes planes(zMin, zMax, delta);
	// Slice
	Slices shellSlices(planes.getPlanesVector().size());
	auto zPlanes = planes.getPlanesVector();
	for (size_t i = 0; i < zPlanes.size(); ++i)
	{
		shellSlices[i].z = zPlanes[i];
	}
	auto sliceTask = subflow.emplace([&models, &planes, &shellSlices](tf::Subflow& childSubFlow) {
		for (auto model : models)
		{
			Slicer::slice(childSubFlow, model->getMesh(), &planes, &shellSlices);
			auto raftSupports = model->supportRaftManager().getRaftSupportMeshes();
			for (auto child : raftSupports)
			{
				Slicer::slice(childSubFlow, child, &planes, &shellSlices);
			}
		}
	});

	auto clippingTask = subflow.emplace([&shellSlices](tf::Subflow& childSubFlow) {
		shellSlices.containmentTreeConstruct(childSubFlow);
	});

	sliceTask.precede(clippingTask);

	qmlManager->setProgress(0.4);

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

	SVGexporter::exportSVG(shellSlices, filename + "_export", isTemp);


	int layer = planes.getPlanesVector().size();
	int time = layer * 15 / 60;
	auto bounds = qmlManager->selectedModelsLengths();
	int64_t area = 0;

	float volume = ((float)(area / pow(scfg->pixel_per_mm / scfg->contraction_ratio, 2)) / 1000000) * scfg->layer_height;
    qmlManager->setProgress(1);
    QStringList name_word = filename.split("/");

    QString size;
    size.sprintf("%.1f X %.1f X %.1f mm", bounds.x(), bounds.y(), bounds.z());
    qmlManager->openResultPopUp("",
                                QString(name_word[name_word.size()-1]+" slicing done.").toStdString(),
                                "");
	return { time , layer, size, volume };
}