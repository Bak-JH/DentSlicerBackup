#pragma once
#include "clipper/clipper.hpp"
#include "clip2tri/clip2tri.h"
#include "poly2tri/poly2tri.h"

class QVector2D;
namespace ClipperLib
{
	static constexpr int INT_PT_RESOLUTION = 1000;
	// converts float point to int in microns
	void  addPoint(float x, float y, ClipperLib::Path* path);
	IntPoint  toInt2DPt(const QVector2D& pt);
	bool ptInPolytree(const QVector2D& pt, const ClipperLib::PolyTree& tree, ClipperLib::PolyNode*& owningNode);
}