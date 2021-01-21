#pragma once
#include "NTreeDeque.h"
#include "../../render/Bounds3D.h"
#include "../../Mesh/mesh.h"
#include <optional>
namespace Hix
{
	namespace Common
	{
		template<typename PriType>
		struct BVHData
		{
			BVHData(Hix::Engine3D::Bounds3D pBound) : bound(pBound){}
			BVHData(Hix::Engine3D::Bounds3D pBound, PriType pPrimitive) : bound(pBound), primitive(pPrimitive){}
			Hix::Engine3D::Bounds3D bound;
			std::optional<PriType> primitive;
		};

		template<typename PriType, template<typename> typename TreeType>
		class BVHTree: public TreeType<NTreeNode<2, BVHData<PriType>>>
		{
		public:
			using NodeData = BVHData<PriType>;
			using BVHNode = NTreeNode<2, NodeData>;
			inline static bool compareCentroidsXCoordinates(const NodeData& a, const NodeData& b)
			{
				return a.bound.centreX() < b.bound.centreX();
			}
			template<typename ItrType>
			Hix::Engine3D::Bounds3D expandBB(const ItrType& begin, const ItrType& last)
			{
				Hix::Engine3D::Bounds3D bound;
				for (auto itr = begin; itr != last; ++itr)
				{
					bound += itr->bound;
				}
				bound += last->bound;
				return bound;
			}
			template<typename ItrType>
			void construct(ItrType begin, ItrType end)
			{
				struct NodeState
				{
					ItrType begin;
					ItrType last;
					BVHNode* currNode;
				};
				std::sort(begin, end, compareCentroidsXCoordinates);
				auto last = end - 1;
				NodeState firstState;
				auto rtBound = expandBB(begin, last);
				
				auto& rootNode = emplaceNode(NodeData(rtBound));
				_root = &rootNode;
				firstState.begin = begin;
				firstState.last = last;
				firstState.currNode = &rootNode;

				std::deque<NodeState> q;
				q.emplace_back(firstState);
				while (!q.empty())
				{
					//auto currState = q.front();
					//q.pop_front();
					auto currState = q.back();
					q.pop_back();
					size_t length = currState.last - currState.begin;
					//qDebug() << "tree state No: " << length;
					if (length == 0)
					{
						currState.currNode->value.primitive = currState.begin->primitive;
					}
					else
					{
						size_t mid = length >> 1;
						auto midItr = currState.begin + mid;
						auto& node0 = emplaceNode(NodeData(expandBB(currState.begin, midItr)));
						auto& node1 = emplaceNode(NodeData(expandBB(midItr + 1, currState.last)));
						currState.currNode->children[0] = &node0;
						currState.currNode->children[1] = &node1;
						NodeState state0{ currState.begin, midItr, &node0 };
						NodeState state1{ midItr + 1, currState.last, &node1 };
						q.emplace_back(state0);
						q.emplace_back(state1);
					}
				}
			}
			void getAll(std::deque<PriType>& all) const
			{
				if (!_root)
					return;

				// Start the intersection process at the root
				std::list<BVHNode*> working_list;
				working_list.push_back(_root);

				while (!working_list.empty())
				{
					auto node = working_list.front();
					working_list.pop_front();
					// We have to check the children of the intersected 'node'
					if (node->hasChildren())
					{
						working_list.push_back(node->children[0]);
						working_list.push_back(node->children[1]);
					}
					else // 'node' is a leaf -> save it's object in the output list
					{
						all.push_back(node->value.primitive.value());
					}
				}

				return;
			}

		};


		namespace BVHBuilder
		{
			BVHTree<Hix::Engine3D::FaceConstItr, Hix::Common::NTreeDeque> build(const Hix::Render::SceneEntity& entity);
		}
	}
}