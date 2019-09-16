#pragma once
#include "clipper/clipper.hpp"
#include "poly2tri/poly2tri.h"

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

		ClipperLib::Path toCLPath(const std::vector<QVector2D>& path);
		ClipperLib::Path toCLPath(const std::vector<QVector3D>& path);

		bool ptInPolytree(const QVector2D& pt, const ClipperLib::PolyTree& tree, ClipperLib::PolyNode*& owningNode);
	}

}