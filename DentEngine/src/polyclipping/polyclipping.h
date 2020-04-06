#pragma once
#include "clipper/clipper.hpp"
#include "poly2tri/poly2tri.h"
#include "../../../feature/agCDT/CDT.h"

class QVector2D;
class QVector3D;


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
	template<>
	struct hash<CDT::V2d<double>>
	{
		//2D only!
		std::size_t operator()(const CDT::V2d<double>& pt)const
		{
			return std::hash<double>()(pt.x) ^ std::hash<double>()(pt.y);
		}
	};
	template<>
	struct hash<std::array<CDT::V2d<double>, 2>> {
		std::size_t operator() (const std::array<CDT::V2d<double>, 2>& key) const {
			return std::hash<CDT::V2d<double>>()(key[0]) ^ std::hash<CDT::V2d<double>>()(key[1]);
		}
	};
}


namespace Hix
{
	namespace Polyclipping
	{



		static constexpr double  INT_PT_RESOLUTION = 2048; //2^11
		//static constexpr double  INT_PT_RESOLUTION = 10000;
		//static constexpr double  INT_PT_RESOLUTION = 134217728; //2^11

		static constexpr double FLOAT_PT_RESOLUTION = 1.0 / INT_PT_RESOLUTION;
		// converts float point to int in microns
		void  addPoint(float x, float y, ClipperLib::Path* path);
		ClipperLib::IntPoint  toInt2DPt(const QVector2D& pt);
		ClipperLib::IntPoint  toPixelSize(const QVector2D& pt);
		ClipperLib::IntPoint  toInt2DPt(const QVector3D& pt);
		QVector2D toFloatPt(const ClipperLib::IntPoint& pt);
		CDT::V2d<double> toDPt(const ClipperLib::IntPoint& pt);

		ClipperLib::Path toCLPath(const std::vector<QVector2D>& path);
		ClipperLib::Path toCLPath(const std::vector<QVector3D>& path);
		ClipperLib::Path toDebugPixelPath(const std::vector<QVector2D>& path);

		bool ptInPolytree(const QVector2D& pt, const ClipperLib::PolyTree& tree, ClipperLib::PolyNode*& owningNode);
	}

}
