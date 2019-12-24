#pragma once
#include "NTree.h"
#include <deque>
namespace Hix
{
	namespace Common
	{
		template<typename NodeType>
		class NTreeDeque: public NTree<NodeType>
		{
		public:
			//can only be moved, not copied due to pointer validity
			NTreeDeque() = default;
			NTreeDeque(const NTreeDeque& other) = delete;
			NTreeDeque(NTreeDeque&& other) = default;
			NTreeDeque& operator=(NTreeDeque other) = delete;
			NTreeDeque& operator=(NTreeDeque&& other) = default;
			virtual ~NTreeDeque() {}
			NodeType& emplaceNode(const typename NodeType::ValueType& value)override
			{
				return _container.emplace_back(value);
			}
			void insert(const typename NodeType::ValueType& key)override
			{
			}
			NodeType& search(const typename NodeType::ValueType& val)override
			{
				return *_root;
			}
			void clear()override
			{
				_container.clear();
				_root = nullptr;
			}
			NodeType* root()override
			{
				return _root;
			}


		protected:
			std::deque <NodeType> _container;
			NodeType* _root = nullptr;
		};
	}
}