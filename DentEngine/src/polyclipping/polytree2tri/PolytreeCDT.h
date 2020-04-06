#pragma once
#include "../polyclipping.h"
#include <qvector2d.h>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <array>
#include "../../../common/RandomGen.h"
namespace Hix
{
	namespace Engine3D
	{
		class Mesh;
	}
}

namespace Hix
{
	namespace Polyclipping
	{
		typedef std::array<QVector2D, 3> Triangle;


		class PolytreeCDT
		{

		public:
			PolytreeCDT(const ClipperLib::PolyTree* polytree);
			PolytreeCDT(const std::unordered_map<ClipperLib::IntPoint, QVector2D>* map, const ClipperLib::PolyTree* polytree);
			std::unordered_map<ClipperLib::PolyNode*, std::vector<Triangle>> triangulate();
			~PolytreeCDT();

		private:
			const ClipperLib::PolyTree* _tree = nullptr;
			const std::unordered_map<ClipperLib::IntPoint, QVector2D>* _floatIntMap = nullptr;

		};



	}

}

