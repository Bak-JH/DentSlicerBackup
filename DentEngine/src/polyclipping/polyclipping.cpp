#include "polyclipping.h"
#include <QVector2D>
#include <QVector3D>
#include <unordered_set>
using namespace ClipperLib;


// converts float point to int in microns
void  ClipperLib::addPoint(float x, float y, ClipperLib::Path* path)
{
	IntPoint ip;
	ip.X = round(x * ClipperLib::INT_PT_RESOLUTION);
	ip.Y = round(y * ClipperLib::INT_PT_RESOLUTION);
	//qDebug() << "addPoint called with x " << x << " y " << y << " rounding " << ip.X;
	path->push_back(ip);
}
IntPoint  ClipperLib::toInt2DPt(const QVector2D& pt)
{
	IntPoint ip;
	ip.X = round(pt.x() * ClipperLib::INT_PT_RESOLUTION);
	ip.Y = round(pt.y() * ClipperLib::INT_PT_RESOLUTION);
	return ip;
}

IntPoint  ClipperLib::toInt2DPt(const QVector3D& pt)
{
	IntPoint ip;
	ip.X = round(pt.x() * ClipperLib::INT_PT_RESOLUTION);
	ip.Y = round(pt.y() * ClipperLib::INT_PT_RESOLUTION);
	return ip;
}
QVector2D ClipperLib::toFloatPt(const IntPoint& pt)
{
	QVector2D fp((float)pt.X/ (float)ClipperLib::INT_PT_RESOLUTION, (float)pt.Y / (float)ClipperLib::INT_PT_RESOLUTION);
	return fp;
}
Path ClipperLib::toCLPath(const std::vector<QVector2D>& path)
{
	Path clipperPath;
	clipperPath.reserve(path.size());
	for (auto& each : path)
	{
		clipperPath.emplace_back(toInt2DPt(each));
	}
	return clipperPath;
}

Path ClipperLib::toCLPath(const std::vector<QVector3D>& path)
{
	Path clipperPath;
	clipperPath.reserve(path.size());
	for (auto& each : path)
	{
		clipperPath.emplace_back(toInt2DPt(each));
	}
	return clipperPath;
}

bool ClipperLib::ptInPolytree(const QVector2D& pt, const ClipperLib::PolyTree& tree, ClipperLib::PolyNode*& owningNode)
{
	IntPoint intPt = toInt2DPt(pt);
	bool lastResultHole = true;
	//because ClipperLib contains non const pointers
	//DFS
	std::vector<PolyNode*> s;
	for (auto& initialChild : tree.Childs)
	{
		if (ClipperLib::PointInPolygon(intPt, initialChild->Contour) == 1)
		{
			s.push_back(initialChild);
		}

	}
	PolyNode* current;
	while (!s.empty())
	{
		current = s.back();
		s.pop_back();
		lastResultHole = current->IsHole();
		if (!lastResultHole)
		{
			owningNode = current;
		}
		for (auto& each : current->Childs)
		{
			if (ClipperLib::PointInPolygon(intPt, current->Contour) == 1)
			{
				s.push_back(each);
			}
		}
	}
	return !lastResultHole;
}

