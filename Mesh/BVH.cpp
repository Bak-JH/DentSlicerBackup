#include "BVH.h"
#include "../glmodel.h"
#include "../../render/Bounds3D.h"
#include "../common/Tree/BVHBuilder.h"
#include "feature/sampling.h"

using namespace Hix;
using namespace Hix::Engine3D;
typedef Hix::Temp::BVH<FaceConstItr> BVHImpl;


class BoundedObjectFactory
{
public:
	BoundedObjectFactory(const std::unordered_map<VertexConstItr, QVector3D>& wPosCache);
	std::vector<BVHImpl::BoundedObject*> getBounds(const std::unordered_set<const GLModel*>& models);
	std::vector<BVHImpl::BoundedObject*> getBounds(const Mesh& mesh);

private:
	BVHImpl::BoundedObject* toBoundedObjectHeap(const FaceConstItr& face);

	const std::unordered_map<VertexConstItr, QVector3D>& _wPosCache;
};






BoundedObjectFactory::BoundedObjectFactory(const std::unordered_map<VertexConstItr, QVector3D>& wPosCache): _wPosCache(wPosCache)
{
}


BVHImpl::BoundedObject* BoundedObjectFactory::toBoundedObjectHeap(const FaceConstItr& face)
{
	auto vs = face.meshVertices();
	Bounds3D bound;
	for (auto& each : vs)
	{
		auto wPos = _wPosCache.at(each);
		bound.update(wPos);
	}
	auto boundedObj = new BVHImpl::BoundedObject(face);
	auto boundPointer = boundedObj->getBounds();
	auto& rawBound = bound.bound();
	auto centoidPtr = boundedObj->getCentroid();
	auto centoid = bound.centre();
	//hix: max min, pcl: min max
	boundPointer[0] = rawBound[1];
	boundPointer[1] = rawBound[0];
	boundPointer[2] = rawBound[3];
	boundPointer[3] = rawBound[2];
	boundPointer[4] = rawBound[5];
	boundPointer[5] = rawBound[4];
	centoidPtr[0] = centoid.x();
	centoidPtr[1] = centoid.y();
	centoidPtr[2] = centoid.z();
	return boundedObj;

}

std::vector<BVHImpl::BoundedObject*> BoundedObjectFactory::getBounds(const std::unordered_set<const GLModel*>& models)
{
	std::vector<BVHImpl::BoundedObject*> bounds;
	size_t faceCount = 0;
	for (auto model : models)
	{
		faceCount += model->getMesh()->getFaces().size();
	}
	bounds.reserve(faceCount);
	for (auto model : models)
	{
		auto faceEnd = model->getMesh()->getFaces().cend();
		for (auto faceItr = model->getMesh()->getFaces().cbegin(); faceItr != faceEnd; ++faceItr)
		{
			bounds.push_back(toBoundedObjectHeap(faceItr));
		}
	}
	return bounds;
}

std::vector<BVHImpl::BoundedObject*> BoundedObjectFactory::getBounds(const Mesh& mesh)
{
	std::vector<BVHImpl::BoundedObject*> bounds;
	size_t faceCount = 0;
		faceCount += mesh.getFaces().size();

	bounds.reserve(faceCount);
		auto faceEnd = mesh.getFaces().cend();
		for (auto faceItr = mesh.getFaces().cbegin(); faceItr != faceEnd; ++faceItr)
		{
			bounds.push_back(toBoundedObjectHeap(faceItr));
		}
	
	return bounds;
}

Hix::Engine3D::BVH::~BVH()
{
	for (auto each : _boundedObjects)
	{
		delete each;
	}
}


std::deque<FaceConstItr> Hix::Engine3D::BVH::getRayCandidates(const QVector3D& rayFrom, const QVector3D& rayTo)
{
	Bounds3D bound;
	bound.update(rayFrom);
	bound.update(rayTo);
	auto& rawBound = bound.bound();
	float pclOrderBound[6];
	pclOrderBound[0] = rawBound[1];
	pclOrderBound[1] = rawBound[0];
	pclOrderBound[2] = rawBound[3];
	pclOrderBound[3] = rawBound[2];
	pclOrderBound[4] = rawBound[5];
	pclOrderBound[5] = rawBound[4];
	////hix: max min, pcl: min max


	QVector3D rayDirection(rayTo - rayFrom);
	rayDirection.normalize();
	std::deque<FaceConstItr> intersected_objects;
	QVector3D dirFrac(1.0f / rayDirection.x(), 1.0f / rayDirection.y(), 1.0f / rayDirection.z());
	if (!root_)
		return intersected_objects;

	bool got_intersection = false;
	// Start the intersection process at the root
	std::list<Node*> working_list;
	working_list.push_back(root_);

	while (!working_list.empty())
	{
		Node* node = working_list.front();
		working_list.pop_front();

		// Is 'node' intersected by the box?
		if (node->intersect(pclOrderBound) && rayIntersectsAABB(dirFrac, rayFrom, *node))
		{
			// We have to check the children of the intersected 'node'
			if (node->hasChildren())
			{
				working_list.push_back(node->getLeftChild());
				working_list.push_back(node->getRightChild());
			}
			else // 'node' is a leaf -> save it's object in the output list
			{
				intersected_objects.push_back(node->getObject()->getData());
				got_intersection = true;
			}
		}
	}

	return intersected_objects;
}

std::pair<float, QVector3D> Hix::Engine3D::BVH::getClosestDistance(const QVector3D& point)
{
	struct LeafInfo
	{
		float dist;
		Node* leafnode;
		QVector3D closestPt;
	};

	std::priority_queue< std::pair<float, Node*>, std::vector<std::pair<float, Node*>>, std::greater<std::pair<float, Node*>> > frontier;
	LeafInfo closestLeaf = {std::numeric_limits<float>::max(), nullptr, QVector3D()};

	frontier.push({ root_->distanceSquared(point), root_ });

	while (!frontier.empty())
	{
		//get current
		auto currentItr = frontier.top();
		Node* currentNode = currentItr.second;
		// exit condition on here //

		if (currentItr.second != nullptr && closestLeaf.dist < currentItr.first)
		{
			auto normalST = point - closestLeaf.closestPt;
			normalST.normalize();
			auto dot = QVector3D::dotProduct(normalST, closestLeaf.leafnode->getObject()->getData().localFn());
			
			return { closestLeaf.dist, closestLeaf.closestPt };
		}

		frontier.pop();

		// expends children
		auto nodeChildren = currentNode->getChildren();

		for (auto idx = 0; idx < 2; ++idx)
		{
			if (nodeChildren[idx]->isLeaf())
			{
				auto closestPt = getClosestVertex(point, nodeChildren[idx]->getObject()->getData());
				auto dist = (point - closestPt).lengthSquared();
				if (dist < closestLeaf.dist)
				{
					closestLeaf.dist = dist;
					closestLeaf.leafnode = nodeChildren[idx];
					closestLeaf.closestPt = closestPt;
				}
			}
			else
			{
				auto dist = nodeChildren[idx]->distanceSquared(point);
				frontier.push({ nodeChildren[idx]->distanceSquared(point), nodeChildren[idx] });
			}
		}

	}
	return { closestLeaf.dist, closestLeaf.closestPt };
}

std::deque<FaceConstItr> Hix::Engine3D::BVH::getRayCandidatesDirection(const QVector3D& rayFrom, const QVector3D& rayDirection)
{
	std::deque<FaceConstItr> intersected_objects;
	QVector3D dirFrac(1.0f / rayDirection.x(), 1.0f / rayDirection.y(), 1.0f / rayDirection.z());
	if (!root_)
		return intersected_objects;

	bool got_intersection = false;
	// Start the intersection process at the root
	std::list<Node*> working_list;
	working_list.push_back(root_);

	while (!working_list.empty())
	{
		Node* node = working_list.front();
		working_list.pop_front();

		// Is 'node' intersected by the box?
		if (rayIntersectsAABB(dirFrac, rayFrom, *node))
		{
			// We have to check the children of the intersected 'node'
			if (node->hasChildren())
			{
				working_list.push_back(node->getLeftChild());
				working_list.push_back(node->getRightChild());
			}
			else // 'node' is a leaf -> save it's object in the output list
			{
				intersected_objects.push_back(node->getObject()->getData());
				got_intersection = true;
			}
		}
	}

	return intersected_objects;
}

QVector3D Hix::Engine3D::BVH::getCachedPos(const VertexConstItr& vtx)const
{
	return _wPosCache.at(vtx);
}

bool Hix::Engine3D::BVH::rayIntersectsAABB(const QVector3D& dirFrac, const QVector3D& orig, const Hix::Temp::BVH<FaceConstItr>::Node& node)const
{
	// lb is the corner of AABB with minimal coordinates - left bottom, rt is maximal corner
// r.org is origin of ray
	auto bound = node.getBound();
	float t1 = (bound[0] - orig.x()) * dirFrac.x();
	float t2 = (bound[1] - orig.x()) * dirFrac.x();
	float t3 = (bound[2] - orig.y()) * dirFrac.y();
	float t4 = (bound[3] - orig.y()) * dirFrac.y();
	float t5 = (bound[4] - orig.z()) * dirFrac.z();
	float t6 = (bound[5] - orig.z()) * dirFrac.z();

	float tmin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
	float tmax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));

	//t is the length of ray until intersection
	// if tmax < 0, ray (line) is intersecting AABB, but the whole AABB is behind us
	if (tmax < 0)
	{
		//t = tmax;
		return false;
	}

	// if tmin > tmax, ray doesn't intersect AABB
	if (tmin > tmax)
	{
		//t = tmax;
		return false;
	}

	//t = tmin;
	return true;
}

Hix::Engine3D::BVH::BVH(const GLModel& model, bool isWorld) : _isWorld(isWorld)
{
	//cache world vertex positions and triangle bounding boxes
	std::unordered_set<const GLModel*> allModels;
	model.getChildrenModels(allModels);
	allModels.insert(&model);
	for (auto each : allModels)
	{
		initModel(*each);
	}
	BoundedObjectFactory boundFactory(_wPosCache);
	_boundedObjects = boundFactory.getBounds(allModels);
	//do not call parent::build, without Node factory patten, defaults to pcl::...Node class
	build(_boundedObjects);

}

Hix::Engine3D::BVH::BVH(const Mesh& mesh, bool isWorld) : _isWorld(isWorld)
{	
	initModel(mesh);
	BoundedObjectFactory boundFactory(_wPosCache);
	_boundedObjects = boundFactory.getBounds(mesh);
	//do not call parent::build, without Node factory patten, defaults to pcl::...Node class
	build(_boundedObjects);

}

Hix::Engine3D::BVH::BVH(const std::unordered_set<const GLModel*>& model, bool isWorld) : _isWorld(isWorld)
{
	//cache world vertex positions and triangle bounding boxes
	for (auto each : model)
	{
		initModel(*each);
	}
	BoundedObjectFactory boundFactory(_wPosCache);
	_boundedObjects = boundFactory.getBounds(model);
	//do not call parent::build, without Node factory patten, defaults to pcl::...Node class
	build(_boundedObjects);
}



void Hix::Engine3D::BVH::initModel(const GLModel& model)
{
	auto vtxEnd = model.getMesh()->getVertices().cend();
	for (auto vtxItr = model.getMesh()->getVertices().cbegin(); vtxItr != vtxEnd; ++vtxItr)
	{
		if (_isWorld)
		{
			_wPosCache[vtxItr] = vtxItr.worldPosition();

		}
		else
		{
			_wPosCache[vtxItr] = vtxItr.localPosition();
		}
	}
}

void Hix::Engine3D::BVH::initModel(const Mesh& mesh)
{
	auto vtxEnd = mesh.getVertices().cend();
	for (auto vtxItr = mesh.getVertices().cbegin(); vtxItr != vtxEnd; ++vtxItr)
	{
		if (_isWorld)
		{
			_wPosCache[vtxItr] = vtxItr.worldPosition();

		}
		else
		{
			_wPosCache[vtxItr] = vtxItr.localPosition();
		}
	}
}

