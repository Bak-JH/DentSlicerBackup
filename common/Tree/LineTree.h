#pragma once
#include "NTreeDeque.h"
#include <QVector3D>
#include "../../render/Bounds3D.h"
namespace Hix
{
	namespace Common
	{
		class LineTree : public NTreeDeque<NTreeNode<2,QVector3D>>
		{
		public:
			virtual ~LineTree();
			template<typename ItrType>
			void construct(ItrType itr, ItrType end)
			{

			}
		};
	}
}


