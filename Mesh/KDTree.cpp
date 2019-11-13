#include "KDTree.h"
#include "../glmodel.h"
using namespace Hix;
using namespace Hix::Engine3D;
Hix::Engine3D::KDTree::KDTree(const GLModel& model):_model(model)
{
	//cache world vertex positions and triangle bounding boxes
	std::unordered_set<const GLModel*> allModels;
	_model.getChildrenModels(allModels);
	allModels.insert(&_model);
	for (auto each : allModels)
	{
		initModel(*each);
	}
	//actually calculate kdtree
	std::vector<FaceConstItr> tris;
	tris.reserve(_triBounds.size());
	for (auto& each : _triBounds)
	{
		tris.push_back(each.first);
	}
	buildTreeRecursive(tris);

}

void Hix::Engine3D::KDTree::initModel(const GLModel& model)
{
	auto vtxEnd = model.getMesh()->getVertices().cend();
	for (auto vtxItr = model.getMesh()->getVertices().cbegin(); vtxItr != vtxEnd; ++vtxItr)
	{
		_wPosCache[vtxItr] = vtxItr.worldPosition();
	}
	auto faceEnd = model.getMesh()->getFaces().cend();
	for (auto faceItr = model.getMesh()->getFaces().cbegin(); faceItr != faceEnd; ++faceItr)
	{
		auto vs = faceItr.meshVertices();
		Bounds3D bound;
		for (auto& each : vs)
		{
			auto wPos = _wPosCache[each];
			bound.update(wPos);
		}
		_triBounds[faceItr] = bound;
	}
}

void Hix::Engine3D::KDTree::buildTreeRecursive(const std::vector<FaceConstItr>& tris)
{
	if (tris.empty())
		return;
	if(tris.size(())
	auto& curr = _nodeContainer.emplace_back();

}
