#pragma once
#include "../polyclipping.h"
#include <qvector2d.h>
#include <unordered_map>
#include <unordered_set>
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
			std::vector<p2t::Point*>& toFloatPts(ClipperLib::PolyNode& node);
			std::vector<p2t::Point*>& toFloatPtsImpl(ClipperLib::PolyNode& node);
			std::vector<p2t::Point*>& toFloatPtsWithMap(ClipperLib::PolyNode& node);

			//points MUST BE UNIQUE!
			//this is map because p2t::point must be non-const
			std::unordered_map<Hix::Polyclipping::Point, p2t::Point> _points;
			//this needs to be maintained as it owns the triangles after triangulation
			std::unordered_map<ClipperLib::PolyNode*,  std::vector<p2t::Point*>> _nodeFloatCache;

			const ClipperLib::PolyTree* _tree = nullptr;
			const std::unordered_map<ClipperLib::IntPoint, QVector2D>* _floatIntMap = nullptr;
		};


	}

}

