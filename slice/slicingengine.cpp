#include "slicingengine.h"

#include <QDir>
#include <QObject>
#include <QDebug>
#include <QProcess>
#include <execution>
#include "glmodel.h"

#include "DentEngine/src/Planes.h"
#include "feature/overhangDetect.h"
#include "DentEngine/src/SlicerDebug.h"
#include "DentEngine/src/SlicerDebugInfoExport.h"
#include "../support/SupportRaftManager.h"
#include "../support/RaftModel.h"
using namespace Hix;
using namespace Hix::Slicer;
using namespace Hix::Render;

std::vector<Hix::Slicer::Slice> SlicingEngine::sliceModels(const std::unordered_set<GLModel*>& models, const Hix::Support::SupportRaftManager& suppRaft, float delta) {
	std::unordered_set<const SceneEntity*> entities;
	for (auto& m : models)
	{
		entities.emplace(m);
	}
	for (auto& support : suppRaft.supportModels())
	{
		entities.emplace(support);
	}
	auto raft = suppRaft.raftModel();
	if (raft)
	{
		entities.emplace(raft);
	}
	return sliceEntities(entities, delta);
}

std::vector<Hix::Slicer::Slice> SlicingEngine::sliceEntities(const std::unordered_set<const SceneEntity*>& models, float delta)
{
	//due to float error with models
	constexpr float BOTT = 0.00001f;
	auto bound = Hix::Engine3D::combineBounds(models);
	UniformPlanes planes(BOTT, bound.zMax(), delta);
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
		Slicer::slice(*model, planes, shellSlices);
	}

#ifdef _DEBUG
	if (Hix::Slicer::Debug::SlicerDebug::getInstance().enableDebug)
	{
		for (size_t i = 0; i < shellSlices.size(); ++i)
		{
			Hix::Slicer::Debug::outDebugSVGs(shellSlices[i].closedContours, i);
			Hix::Slicer::Debug::outDebugIncompletePathsSVGs(shellSlices[i].incompleteContours, i);
		}
	}
#endif
	//use clipper to combine clippings
	containmentTreeConstruct(shellSlices);
	//remove empty contours from the top and bottom
	size_t forwardPopCnt = 0;
	auto forwardItr = shellSlices.begin();
	while (forwardItr->polytree->ChildCount() == 0)
	{
		++forwardPopCnt;
		++forwardItr;
	}
	shellSlices.erase(shellSlices.begin(), shellSlices.begin() + forwardPopCnt);
	size_t backPopCnt = 0;
	auto backItr = shellSlices.rbegin();
	while (backItr->polytree->ChildCount() == 0)
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
