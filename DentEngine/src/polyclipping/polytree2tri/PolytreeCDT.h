#pragma once
#include "../clipper/clipper.hpp"
#include "../poly2tri/poly2tri.h"
#include <qvector2d.h>
#include <unordered_map>
#include <vector>
#include <array>
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
		class PolytreeCDT
		{

		public:
			typedef std::array<QVector2D, 3> Triangle;
			PolytreeCDT(const ClipperLib::PolyTree* polytree);
			PolytreeCDT(const std::unordered_map<ClipperLib::IntPoint, QVector2D>* map, const ClipperLib::PolyTree* polytree);
			std::unordered_map<ClipperLib::PolyNode*, std::vector<Triangle>> triangulate();
			~PolytreeCDT();

		private:
			void toFloatPts();
			void toFloatPtsWithMap();
			//this needs to be maintained as it owns the triangles after triangulation
			std::unordered_map<ClipperLib::PolyNode*, std::vector<p2t::Point*>> _nodeFloatPtMap;

			const ClipperLib::PolyTree* _tree = nullptr;
			const std::unordered_map<ClipperLib::IntPoint, QVector2D>* _floatIntMap = nullptr;
		};


	}

}

