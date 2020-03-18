//#include "slicingengine.h"
#include <QDir>
#include <QObject>
#include <QDebug>
#include <QProcess>
#include "slicingengine.h"

#include "glmodel.h"

#include "DentEngine/src/Planes.h"
#include "feature/overhangDetect.h"
#include "DentEngine/src/SlicerDebug.h"
#include "DentEngine/src/SlicerDebugInfoExport.h"
#include "../support/SupportRaftManager.h"
#include "../application/ApplicationManager.h"
using namespace Hix;
using namespace Hix::Slicer;
using namespace Hix::Render;

std::vector<Hix::Slicer::Slice> SlicingEngine::sliceModels(float zMax,
	const std::unordered_set<GLModel*>& models, const Hix::Support::SupportRaftManager& suppRaft){
	constexpr float BOTT = 0.0f;
	//generate planes
	//if (scfg->slicing_mode == SlicingConfiguration::SlicingMode::Uniform) {
	float delta = Hix::Application::ApplicationManager::getInstance().settings().sliceSetting.layerHeight;
	UniformPlanes planes(BOTT, zMax, delta);
	std::vector<Slice> shellSlices(planes.getPlanesVector().size());
	auto zPlanes = planes.getPlanesVector();
	//set z elevation for each slizes
	for (size_t i = 0; i < zPlanes.size(); ++i)
	{
		shellSlices[i].z = zPlanes[i];
	}
	//slice models
	for (auto& model : models)
	{
		Slicer::slice(dynamic_cast<const SceneEntity&>(*model), planes, shellSlices);
	}
	//slice supports
	for (auto& support : suppRaft.supportModels())
	{
		Slicer::slice(support, planes, shellSlices);
	}
	//slice raft
	auto raft = suppRaft.raftModel();
	if (raft)
	{
		Slicer::slice(*raft, planes, shellSlices);
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
	containmentTreeConstruct(shellSlices);
	//Hix::Application::ApplicationManager::getInstance().setProgress(0.4);

	//remove empty contours from the top and bottom
	size_t forwardPopCnt = 0;
	auto forwardItr = shellSlices.begin();
	while (forwardItr->polytree.ChildCount() == 0)
	{
		++forwardPopCnt;
		++forwardItr;
	}
	shellSlices.erase(shellSlices.begin(), shellSlices.begin() + forwardPopCnt);
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

	return shellSlices;
}