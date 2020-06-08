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

std::vector<Hix::Slicer::LayerGroup> SlicingEngine::sliceModels(const std::unordered_set<GLModel*>& models, const Hix::Support::SupportRaftManager& suppRaft, float delta) {
	auto entities = selectedToEntities(models, suppRaft);
	return sliceEntities(entities, delta);
}


std::unordered_set<const SceneEntity*> SlicingEngine::selectedToEntities(const std::unordered_set<GLModel*>& models, const Hix::Support::SupportRaftManager& suppRaft)
{
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
	return entities;
}

std::vector<Hix::Slicer::LayerGroup> SlicingEngine::sliceEntities(const std::unordered_set<const SceneEntity*>& models, float delta)
{
	//due to float error with models
	constexpr float BOTT = 0.00001f;
	auto bound = Hix::Engine3D::combineBounds(models);
	UniformPlanes planes(BOTT, bound.zMax(), delta);
	size_t layerCnt = planes.getPlanesVector().size();
	std::vector<LayerGroup> layers(layerCnt);
	auto zPlanes = planes.getPlanesVector();
	//add all children, so we don't have to slice recursively
	std::unordered_set<const SceneEntity*> modelsAndChildren;
	for (auto m : models)
	{
		m->getChildrenModels(modelsAndChildren);
		modelsAndChildren.insert(m);
	}

	//set z elevation for each slizes
	for (size_t i = 0; i < zPlanes.size(); ++i)
	{
		layers[i].z = zPlanes[i];
	}
	//slice models
	for (auto& model : modelsAndChildren)
	{
		auto modelSlices = Slicer::slice(*model, planes);
		//move individual models slices to total slice
		for (size_t i = 0; i < layerCnt; ++i)
		{
			auto& total = layers[i].slices;
			auto& from = modelSlices[i].slices;
			std::move(from.begin(), from.end(), std::back_inserter(total));
		}
	}

#ifdef _DEBUG
	if (Hix::Slicer::Debug::SlicerDebug::getInstance().enableDebug)
	{
		for (size_t i = 0; i < layers.size(); ++i)
		{
			Hix::Slicer::Debug::outDebugSVGs(layers[i], i);
			Hix::Slicer::Debug::outDebugIncompletePathsSVGs(layers[i], i);
		}
	}
#endif
	//use clipper to combine clippings
	for (auto& l : layers)
	{
		for (auto& s : l.slices)
		{
			containmentTreeConstruct(s);

		}
	}
	//remove empty contours from the top and bottom
	size_t forwardPopCnt = 0;
	auto forwardItr = layers.begin();
	while (forwardItr->empty())
	{
		++forwardPopCnt;
		++forwardItr;
	}
	layers.erase(layers.begin(), layers.begin() + forwardPopCnt);
	size_t backPopCnt = 0;
	auto backItr = layers.rbegin();

	while (backItr->empty())
	{
		++backPopCnt;
		++backItr;
	}

	for (size_t i = 0; i < backPopCnt; ++i)
	{
		layers.pop_back();
	}
	qDebug() << "removed empty slices count bott: " << forwardPopCnt << "top :" << backPopCnt;
	qDebug() << "after remove: " << layers.size();
	return layers;
}
