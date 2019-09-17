#pragma once
#include "clipper/clipper.hpp"
#include "poly2tri/poly2tri.h"
#include "polytree2tri/PolytreeCDT.h"
class QVector2D;
class QVector3D;

namespace Hix
{
	namespace Polyclipping
	{
		static constexpr int INT_PT_RESOLUTION = 1000;
		// converts float point to int in microns
		void  addPoint(float x, float y, ClipperLib::Path* path);
		ClipperLib::IntPoint  toInt2DPt(const QVector2D& pt);
		ClipperLib::IntPoint  toInt2DPt(const QVector3D& pt);
		QVector2D toFloatPt(const ClipperLib::IntPoint& pt);
		p2t::Point*  toDoublePtHeap(const ClipperLib::IntPoint& pt);

		ClipperLib::Path toCLPath(const std::vector<QVector2D>& path);
		ClipperLib::Path toCLPath(const std::vector<QVector3D>& path);

		bool ptInPolytree(const QVector2D& pt, const ClipperLib::PolyTree& tree, ClipperLib::PolyNode*& owningNode);
	}

}
namespace std
{
	template<>
	struct hash<ClipperLib::IntPoint>
	{
		//2D only!
		std::size_t operator()(const ClipperLib::IntPoint& pt)const
		{
			size_t digest = pt.X | pt.Y << 32;
			return digest;
		}
	};

}