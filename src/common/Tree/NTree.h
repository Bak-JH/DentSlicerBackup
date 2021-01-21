#pragma once
#include<array>
namespace Hix
{
	namespace Common
	{
		//@precondition: requires stable pointer/reference after addition
		template<size_t NCnt, typename ValType>
		struct NTreeNode
		{
			using ValueType = ValType;
			NTreeNode(const ValType& val) :value(val), children{}
			{
			}
			ValType value;
			std::array<NTreeNode*, NCnt> children;
			bool hasChildren()
			{
				if (children[0])
					return true;
				else
					return false;
			}

		};
		template<typename NodeType>
		class NTree
		{
		public:
			virtual ~NTree() {}
			virtual NodeType& emplaceNode(const typename NodeType::ValueType& value) = 0;
			virtual void insert(const typename NodeType::ValueType& key) = 0;
			virtual NodeType& search(const typename NodeType::ValueType& val) = 0;
			virtual void clear() = 0;
			virtual NodeType* root() = 0;
		};
	}
}