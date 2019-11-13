#pragma once
#include "../DentEngine/src/mesh.h"
class GLModel;
namespace Hix
{
	namespace Engine3D
	{
		struct KDNode
		{
			Bounds3D aabb;
			KDNode* left = nullptr;
			KDNode* right = nullptr;
		};

		class KDTree
		{
		public:
			KDTree(const GLModel& model);
			void initModel(const GLModel& model);
		private:
			void buildTreeRecursive(const std::vector<FaceConstItr>& tris);
			const GLModel& _model;
			std::unordered_map<FaceConstItr, Bounds3D> _triBounds;
			std::unordered_map<VertexConstItr, QVector3D> _wPosCache;
			std::deque<KDNode> _nodeContainer;
		};

	}
}


