#include "BVHBuilder.h"
#include "../../render/SceneEntity.h"
using namespace Hix::Common;
using namespace Hix::Engine3D;
BVHTree<Hix::Engine3D::FaceConstItr, Hix::Common::NTreeDeque> Hix::Common::BVHBuilder::build(const Hix::Render::SceneEntity& entity)
{
	BVHTree<Hix::Engine3D::FaceConstItr, Hix::Common::NTreeDeque> tree;
	//auto mesh = entity.getMesh();
	//auto worldPosCache = mesh->cacheWorldPos();
	//std::vector<BVHData<Hix::Engine3D::FaceConstItr>> data;
	//data.reserve(mesh->getFaces().size());
	//auto faceCend = mesh->getFaces().cend();
	//for (auto itr = mesh->getFaces().cbegin(); itr != faceCend; ++itr)
	//{
	//	Bounds3D bound;
	//	auto mvs = itr.meshVertices();
	//	for (auto& v : mvs)
	//	{
	//		bound.update(worldPosCache[v]);
	//	}
	//	data.emplace_back(bound, itr);
	//}
	//tree.construct(data.begin(), data.end());
	return tree;
}
