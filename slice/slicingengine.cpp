//#include "slicingengine.h"
#include <QDir>
#include <QObject>
#include <QDebug>
#include <QProcess>
#include "DentEngine/src/svgexporter.h"
#include "slicingengine.h"

#include "glmodel.h"

#include "DentEngine/src/slicer.h"
#include "DentEngine/src/Planes.h"
#include "feature/overhangDetect.h"
#include "DentEngine/src/SlicerDebug.h"
#include "DentEngine/src/SlicerDebugInfoExport.h"
#include "../support/SupportRaftManager.h"
#include "../application/ApplicationManager.h"
using namespace Hix;
using namespace Hix::Slicer;
using namespace Hix::Render;

SlicingEngine::Result SlicingEngine::sliceModels(bool isTemp, float zMax,
	const std::unordered_set<GLModel*>& models, const Hix::Support::SupportRaftManager& suppRaft, QString filename){

	constexpr float BOTT = 0.0f;

    //Hix::Application::ApplicationManager::getInstance().setProgress(0.1);

	//generate planes
	//if (scfg->slicing_mode == SlicingConfiguration::SlicingMode::Uniform) {
	float delta = Hix::Application::ApplicationManager::getInstance().settings().sliceSetting.layerHeight;
	UniformPlanes planes(BOTT, zMax, delta);
	Slices shellSlices(planes.getPlanesVector().size());
	auto zPlanes = planes.getPlanesVector();
	//set z elevation for each slizes
	for (size_t i = 0; i < zPlanes.size(); ++i)
	{
		shellSlices[i].z = zPlanes[i];
	}
	filename += "_export";
	QDir dir(filename);
	if (!dir.exists()) {
		dir.mkpath(".");
	}
	else {
		dir.removeRecursively();
		dir.mkpath(".");
	}
	//debug log
	if (Hix::Slicer::Debug::SlicerDebug::getInstance().enableDebug)
	{
		Hix::Slicer::Debug::SlicerDebug::getInstance().debugFilePath = filename + QString("/debug/");
		QDir debugDir(Hix::Slicer::Debug::SlicerDebug::getInstance().debugFilePath);
		if (!debugDir.exists()) {
			debugDir.mkpath(".");
		}
	}
	//slice models
	for (auto& model : models)
	{
		Slicer::slice(dynamic_cast<const SceneEntity&>(*model), &planes, &shellSlices);
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
	//debug svg
	if (Hix::Slicer::Debug::SlicerDebug::getInstance().enableDebug)
	{
		for (size_t i = 0; i < shellSlices.size(); ++i)
		{
			Hix::Slicer::Debug::outDebugSVGs(shellSlices[i].closedContours, i);
			Hix::Slicer::Debug::outDebugIncompletePathsSVGs(shellSlices[i].incompleteContours, i);
		}
	}

	//use clipper to combine clippings
	shellSlices.containmentTreeConstruct();
	//Hix::Application::ApplicationManager::getInstance().setProgress(0.4);

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

	SVGexporter exp;
	exp.exportSVG(shellSlices, filename, isTemp);


	int layer = planes.getPlanesVector().size();
	int time = layer * 15 / 60;
	auto bounds = Hix::Engine3D::combineBounds(models).lengths();
	int64_t area = 0;

	float volume = ((float)(area / pow(Hix::Application::ApplicationManager::getInstance().settings().printerSetting.pixelPerMMX(), 2)) / 1000000) * delta;
    //Hix::Application::ApplicationManager::getInstance().setProgress(1);
    QStringList name_word = filename.split("/");

    QString size;
    size.sprintf("%.1f X %.1f X %.1f mm", bounds.x(), bounds.y(), bounds.z());
    //Hix::Application::ApplicationManager::getInstance().openResultPopUp("",
    //                            QString(name_word[name_word.size()-1]+" slicing done.").toStdString(),
    //                            "");
	return { time , layer, size, volume };
}