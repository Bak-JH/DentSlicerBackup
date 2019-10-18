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
using namespace Hix::Render;

SlicingEngine::Result SlicingEngine::sliceModels(bool isTemp, tf::Subflow& subflow, float zMax, std::vector<std::reference_wrapper<const GLModel>> models, const Hix::Support::SupportRaftManager& suppRaft, QString filename){

	constexpr float BOTT = 0.0f;

    qmlManager->setProgress(0.1);

	//generate planes
	//if (scfg->slicing_mode == SlicingConfiguration::SlicingMode::Uniform) {
	float delta = scfg->layer_height;
	UniformPlanes planes(BOTT, zMax, delta);
	Slices shellSlices(planes.getPlanesVector().size());
	auto zPlanes = planes.getPlanesVector();
	//set z elevation for each slizes
	for (size_t i = 0; i < zPlanes.size(); ++i)
	{
		shellSlices[i].z = zPlanes[i];
	}
	//slice models
	for (auto& model : models)
	{
		Slicer::slice(dynamic_cast<const SceneEntity&>(model.get()), &planes, &shellSlices);
	}
	//slice supports
	for (auto& support : suppRaft.supportModels())
	{
		Slicer::slice(support, &planes, &shellSlices);
	}
	//slice raft
	auto raft = suppRaft.raftModel();
	if (raft)
	{
		Slicer::slice(*raft, &planes, &shellSlices);
	}
	//use clipper to combine clippings
	shellSlices.containmentTreeConstruct();
	qmlManager->setProgress(0.4);

	//remove empty contours from the top and bottom
	size_t forwardPopCnt = 0;
	auto forwardItr = shellSlices.begin();
	while (forwardItr->polytree.ChildCount() == 0)
	{
		++forwardPopCnt;
		++forwardItr;
	}
	for (size_t i = 0; i < forwardPopCnt; ++i)
	{
		shellSlices.pop_front();
	}
	size_t backPopCnt = 0;
	auto backItr = shellSlices.rbegin();
	while (backItr->polytree.ChildCount() == 0)
	{
		++backPopCnt;
		++backItr;
	}
	for (size_t i = 0; i < backPopCnt; ++i)
	{
		shellSlices.pop_back();
	}
	qDebug() << "removed empty slices count bott: " << forwardPopCnt << "top :" << backPopCnt;

    // Export to SVG
    //QString export_info = SVGexporter::exportSVG(shellSlices, supportSlices, raftSlices, filename+"_export", isTemp);

	SVGexporter::exportSVG(shellSlices, filename + "_export", isTemp);


	int layer = planes.getPlanesVector().size();
	int time = layer * 15 / 60;
	auto bounds = qmlManager->selectedModelsLengths();
	int64_t area = 0;

	float volume = ((float)(area / pow(scfg->pixelPerMMX() / scfg->contraction_ratio, 2)) / 1000000) * scfg->layer_height;
    qmlManager->setProgress(1);
    QStringList name_word = filename.split("/");

    QString size;
    size.sprintf("%.1f X %.1f X %.1f mm", bounds.x(), bounds.y(), bounds.z());
    qmlManager->openResultPopUp("",
                                QString(name_word[name_word.size()-1]+" slicing done.").toStdString(),
                                "");
	return { time , layer, size, volume };
}