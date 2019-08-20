#include "polyclipping.h"
#include <QVector2D>
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

