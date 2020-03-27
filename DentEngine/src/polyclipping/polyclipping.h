#pragma once
#include "clipper/clipper.hpp"
#include "poly2tri/poly2tri.h"
class QVector2D;
class QVector3D;

namespace Hix
{
	namespace Polyclipping
	{
		struct Point
		{
			double _x;
			double _y;
			inline double x() const { return _x; }
			inline double y() const { return _y; }
			friend inline bool operator== (const Point& a, const Point& b) {
				return a._x == b._x && a._y == b._y;
			}
			friend inline bool operator!= (const Point& a, const Point& b) {
				return !operator==(a,b);
			}
		};
	}
}
namespace std
{
	template<>
	struct hash<Hix::Polyclipping::Point>
	{
		//2D only!
		std::size_t operator()(const Hix::Polyclipping::Point& pt)const
		{
			//we don't care about symmertricity as the vertice pair should be sorted anyway ie) no symmetry
			return std::hash<double>()(pt._x) ^ std::hash<double>()(pt._y);
		}
	};

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

namespace Hix
{
	namespace Polyclipping
	{



		static constexpr double  INT_PT_RESOLUTION = 131072.0; //2*17
		static constexpr double FLOAT_PT_RESOLUTION = 1.0 / INT_PT_RESOLUTION;
		// converts float point to int in microns
		void  addPoint(float x, float y, ClipperLib::Path* path);
		ClipperLib::IntPoint  toInt2DPt(const QVector2D& pt);
		ClipperLib::IntPoint  toPixelSize(const QVector2D& pt);
		ClipperLib::IntPoint  toInt2DPt(const QVector3D& pt);
		QVector2D toFloatPt(const ClipperLib::IntPoint& pt);
		Hix::Polyclipping::Point toDPt(const ClipperLib::IntPoint& pt);

		ClipperLib::Path toCLPath(const std::vector<QVector2D>& path);
		ClipperLib::Path toCLPath(const std::vector<QVector3D>& path);
		ClipperLib::Path toDebugPixelPath(const std::vector<QVector2D>& path);

		bool ptInPolytree(const QVector2D& pt, const ClipperLib::PolyTree& tree, ClipperLib::PolyNode*& owningNode);
	}

}
