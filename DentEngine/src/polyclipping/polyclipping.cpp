#include "polyclipping.h"
#include <QVector2D>
#include <QVector3D>
#include <unordered_set>
#include "../../../application/ApplicationManager.h"

using namespace ClipperLib;

// representable range
double constexpr min_value = std::numeric_limits<long>::min() / Hix::Polyclipping::INT_PT_RESOLUTION;
double constexpr max_value = std::numeric_limits<long>::max() / Hix::Polyclipping::INT_PT_RESOLUTION;


// converts float point to int in microns
void  Hix::Polyclipping::addPoint(float x, float y, ClipperLib::Path* path)
{

	//qDebug() << "addPoint called with x " << x << " y " << y << " rounding " << ip.X;
	path->push_back(toInt2DPt(QVector2D(x,y)));
}

cInt floatToInt(float v)
{
	if (v < 0)
	{
		return static_cast<cInt>(v * Hix::Polyclipping::INT_PT_RESOLUTION - 0.5);
	}
	else
	{
		return static_cast<cInt>(v * Hix::Polyclipping::INT_PT_RESOLUTION + 0.5);
	}
}

IntPoint  Hix::Polyclipping::toInt2DPt(const QVector2D& pt)
{
	IntPoint ip;
	ip.X = floatToInt(pt.x());
	ip.Y = floatToInt(pt.y());
	return ip;
}

IntPoint Hix::Polyclipping::toPixelSize(const QVector2D& pt)
{
	auto& pSet = Hix::Application::ApplicationManager::getInstance().settings().printerSetting;
	IntPoint pixelSize;
	auto& setting = Hix::Application::ApplicationManager::getInstance().settings().sliceSetting;
	float invert = setting.invertX ? -1.0f : 1.0f;
	pixelSize.X = std::round(pt.x()* invert * pSet.pixelPerMMX()
		+ (pSet.sliceImageResolutionX / 2));

	pixelSize.Y = std::round(pt.y() * pSet.pixelPerMMY()
		+ (pSet.sliceImageResolutionY / 2));
	return pixelSize;
}

IntPoint  Hix::Polyclipping::toInt2DPt(const QVector3D& pt)
{
	return toInt2DPt(pt.toVector2D());
}
QVector2D Hix::Polyclipping::toFloatPt(const IntPoint& pt)
{
	QVector2D fp((float)pt.X/ Hix::Polyclipping::INT_PT_RESOLUTION, (float)pt.Y /Hix::Polyclipping::INT_PT_RESOLUTION);
	return fp;
}


Hix::Polyclipping::Point Hix::Polyclipping::toDPt(const ClipperLib::IntPoint& pt)
{
	return { (double)pt.X / Hix::Polyclipping::INT_PT_RESOLUTION, (double)pt.Y / Hix::Polyclipping::INT_PT_RESOLUTION };
}

Path Hix::Polyclipping::toCLPath(const std::vector<QVector2D>& path)
{
	Path clipperPath;
	clipperPath.reserve(path.size());
	for (auto& each : path)
	{
		clipperPath.emplace_back(toInt2DPt(each));
	}
	return clipperPath;
}

ClipperLib::Path Hix::Polyclipping::toCLPath(const std::vector<QVector3D>& path)
{
	Path clipperPath;
	clipperPath.reserve(path.size());
	for (auto& each : path)
	{
		clipperPath.emplace_back(toInt2DPt(each));
	}
	return clipperPath;
}

ClipperLib::Path Hix::Polyclipping::toDebugPixelPath(const std::vector<QVector2D>& path)
{
	Path clipperPath;
	clipperPath.reserve(path.size());
	for (auto& each : path)
	{
		clipperPath.emplace_back(toPixelSize(each));
	}
	return clipperPath;
}

bool Hix::Polyclipping::ptInPolytree(const QVector2D& pt, const ClipperLib::PolyTree& tree, ClipperLib::PolyNode*& owningNode)
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
