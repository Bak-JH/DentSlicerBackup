#pragma once
#include "RayAccelerator.h"
#include "HixBVH.h"
class GLModel;


namespace Hix
{
	using namespace Hix::Temp;
	namespace Engine3D
	{
		class BVH : protected Hix::Temp::BVH<FaceConstItr>,public  RayAccelerator
		{
		public:
			BVH(const GLModel& model);
			void initModel(const GLModel& model);
			virtual ~BVH();
			std::deque<FaceConstItr> getRayCandidates(const QVector3D& rayFrom, const QVector3D& rayTo)override;
			std::deque<FaceConstItr> getRayCandidatesDirection(const QVector3D& rayFrom, const QVector3D& rayDirection)override;
			QVector3D getWorldPos(const VertexConstItr& vtx)const override;


		private:
			bool rayIntersectsAABB(const QVector3D& dirFac, const QVector3D& orig, const Hix::Temp::BVH<FaceConstItr>::Node& node)const;
			const GLModel& _model;
			std::unordered_map<VertexConstItr, QVector3D> _wPosCache;
			std::vector<Hix::Temp::BVH<FaceConstItr>::BoundedObject*> _boundedObjects;
		};
		
		
		
		





		//class BVH: public RayAccelerator
		//{
		//public:
		//	BVH(const GLModel& model);
		//	void initModel(const GLModel& model);
		//	~BVH();
		//	std::vector<FaceConstItr> getRayCandidates(const QVector3D& rayFrom, const QVector3D& rayTo)override;
		//	QVector3D getWorldPos(const VertexConstItr& vtx)const override;

		//private:
		//	//void buildTreeRecursive(const std::vector<FaceConstItr>& tris);
		//	const GLModel& _model;
		//	//std::unordered_map<FaceConstItr, Bounds3D> _triBounds;
		//	std::unordered_map<VertexConstItr, QVector3D> _wPosCache;
		//	BVHImpl _bvhImpl;
		//	std::vector<BVHImpl::BoundedObject*> _boundedObjects;
		//};


	}
}


