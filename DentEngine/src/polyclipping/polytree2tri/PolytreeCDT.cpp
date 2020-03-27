#include "PolytreeCDT.h"
#include <unordered_set>
#include <qdebug.h>
#include "../../SlicerDebugInfoExport.h"
#include "../../../../application/ApplicationManager.h"
using namespace ClipperLib;
using namespace Hix::Polyclipping;
using namespace Hix::Slicer::Debug;

PolytreeCDT::PolytreeCDT(const ClipperLib::PolyTree* polytree): _tree(polytree)
{
}

PolytreeCDT::PolytreeCDT(const std::unordered_map<ClipperLib::IntPoint, QVector2D>* map,
	const ClipperLib::PolyTree* polytree):
	_tree(polytree), _floatIntMap(map)
{
}

PolytreeCDT::Triangle convertTrig(p2t::Triangle* orig)
{
	PolytreeCDT::Triangle trig;
	for (size_t i = 0; i < 3; ++i)
	{
		auto pt = orig->GetPoint(i);
		trig[i] = QVector2D(pt->x, pt->y);
	}
	return trig;
}

//bool pathContainsDupe(const std::vector<p2t::Point*>& path)
//{
//	std::unordered_set<p2t::Point> set;
//	for (auto e : path)
//	{
//		auto result = set.insert(*e);
//		if (!result.second)
//		{
//			qDebug() << "contains duplicate point in path:" << e->x << e->y;
//			return true;
//		}
//	}
//	return false;
//}
//
//bool pathContainsDupe(const std::vector< IntPoint >& path)
//{
//	std::unordered_set<IntPoint> set;
//	for (auto e : path)
//	{
//		auto result = set.insert(e);
//		if (!result.second)
//		{
//			return true;
//		}
//	}
//	return false;
//}

std::vector<Hix::Polyclipping::Point> debugPath(std::vector<p2t::Point*>& node)
{
	std::vector<Hix::Polyclipping::Point> res;
	res.reserve(node.size());
	for (auto e : node)
	{
		res.push_back({ e->x* 51200 + 1000000, e->y* 51200 + 1000000 });
	}
	return res;
}
bool tooSmallForTrig(const ClipperLib::PolyNode& node, double pixArea)
{
	//auto area = ClipperLib::Area(node.Contour) / std::pow((double)Hix::Polyclipping::INT_PT_RESOLUTION, 2);
	//if (area < pixArea)
	//	return true;
	//if (node.Contour.front() != node.Contour.back())
	//	return true;
	return false;
}

static size_t dSliceCnt = 0;
std::unordered_map<PolyNode*, std::vector<PolytreeCDT::Triangle>> Hix::Polyclipping::PolytreeCDT::triangulate()
{
	double pixelArea = Hix::Application::ApplicationManager::getInstance().settings().printerSetting.pixelSizeX() *
		Hix::Application::ApplicationManager::getInstance().settings().printerSetting.pixelSizeY();
	std::unordered_map<PolyNode*, std::vector<PolytreeCDT::Triangle>> result;

	// find solids
	std::unordered_set<PolyNode*> solids;
	std::deque<PolyNode*> q;
	auto firstSolids = _tree->Childs;
	for (auto each : firstSolids)
	{
		solids.emplace(each);
		q.emplace_back(each);
	}
	while (!q.empty())
	{
		auto curr = q.front();
		q.pop_front();
		auto holes = curr->Childs;
		for (auto& hole : holes)
		{
			auto currSolids = hole->Childs;
			for (auto& solid : currSolids)
			{
				solids.emplace(solid);
				q.emplace_back(solid);
			}
		}
	}

	for (auto& eachSolid : solids)
	{
		if (tooSmallForTrig(*eachSolid, pixelArea))
			continue;

		//auto isSolidDupe = pathContainsDupe(toFloatPts(*eachSolid));
		auto solidF = toFloatPts(*eachSolid);
		if (solidF.empty())
			continue;
		p2t::CDT cdt(solidF);
		//add holes
		for (auto hole : eachSolid->Childs)
		{
			if (tooSmallForTrig(*hole, pixelArea))
				continue;
			//auto isHoleDupe = pathContainsDupe(toFloatPts(*hole));
			auto holeF = toFloatPts(*hole);
			if (holeF.empty())
				continue;
			cdt.AddHole(holeF);
		}
		try
		{
			cdt.Triangulate();
			auto p2tTrigs = cdt.GetTriangles();
			//create vector to contain trigs
			auto& trigVector = result[eachSolid];
			trigVector.reserve(p2tTrigs.size());
			//convert to more robust value trigs
			for (auto& trig : p2tTrigs)
			{
				trigVector.emplace_back(convertTrig(trig));
			}
			//stupid fucking code
			cdt.resetPoints();
		}
		catch (...)
		{

			auto solidPath = debugPath(toFloatPts(*eachSolid));
			//auto bound = Hix::Engine3D::bound2D(solidPath.begin(), solidPath.end());
			//SVGOut output("ssshit", QSize(bound.lengthX() * 100, bound.lengthY() * 100));
			{
				SVGOut output("debug_svg" + std::to_string(dSliceCnt) + "solid", QSize(2000000, 2000000));
				QPen pen(QColor::fromRgb(200, 0, 0), 1, Qt::SolidLine);
				output.addPath(solidPath, pen, "solid");
			}

			size_t cnt = 0;
			for (auto hole : eachSolid->Childs)
			{
				SVGOut output("debug_svg" + std::to_string(dSliceCnt) + "hole" + std::to_string(cnt), QSize(2000000, 2000000));
				std::string name = "hole" + std::to_string(cnt);
				++cnt;
				//auto isHoleDupe = pathContainsDupe(toFloatPts(*hole));
				QPen holePen(output.randColor(), 1, Qt::SolidLine);
				output.addPath(debugPath(toFloatPts(*hole)), holePen, "");
			}
			//QMetaObject::invokeMethod(&Hix::Application::ApplicationManager::getInstance().engine(), [&]() {
			//	}, Qt::BlockingQueuedConnection);


		}

	}
	++dSliceCnt;
	return result;
}


PolytreeCDT::~PolytreeCDT()
{
}

std::vector<p2t::Point*>& Hix::Polyclipping::PolytreeCDT::toFloatPts(ClipperLib::PolyNode& node)
{
	auto cached = _nodeFloatCache.find(&node);
	if (cached != _nodeFloatCache.end())
		return cached->second;

	if (_floatIntMap)
	{
		return toFloatPtsWithMap(node);
	}
	else
	{
		return toFloatPtsImpl(node);
	}
}
//constexpr double MIN_PIX_AREA = 57.9261875148;
constexpr double MIN_PIX_AREA = 5;

std::vector<p2t::Point*>& PolytreeCDT::toFloatPtsImpl(ClipperLib::PolyNode& node)
{
	std::vector<p2t::Point*> floatPath;
	floatPath.reserve(node.Contour.size());
	//pathContainsDupe(node.Contour);
	auto cleanedContour = node.Contour;
	ClipperLib::CleanPolygon(cleanedContour, MIN_PIX_AREA);
	if (cleanedContour != node.Contour)
	{
		qDebug() << "cleaning worked";
	}
	for (auto& point : cleanedContour)
	{
		auto dPoint = toDPt(point);
		auto fpt = _points.insert(std::make_pair(dPoint, p2t::Point(dPoint._x, dPoint._y)));
		floatPath.emplace_back(&fpt.first->second);
	}
	auto inserted = _nodeFloatCache.try_emplace(&node, std::move(floatPath));
	return inserted.first->second;
}

std::vector<p2t::Point*>& PolytreeCDT::toFloatPtsWithMap(ClipperLib::PolyNode& node)
{
	std::vector<p2t::Point*> floatPath;
	floatPath.reserve(node.Contour.size());
	//pathContainsDupe(node.Contour);
	auto cleanedContour = node.Contour;
	ClipperLib::CleanPolygon(cleanedContour, MIN_PIX_AREA);
	if (cleanedContour != node.Contour)
	{
		qDebug() << "cleaning worked";
	}
	for (auto& point : cleanedContour)
	{
		auto origFloatPt = _floatIntMap->find(point);
		if (origFloatPt == _floatIntMap->end())
		{
			//no matching original float point detected
			auto dPoint = toDPt(point);
			auto fpt = _points.insert(std::make_pair(dPoint, p2t::Point(dPoint._x, dPoint._y)));
			floatPath.emplace_back(&fpt.first->second);

		}
		else
		{
			auto qVector = origFloatPt->second;
			Polyclipping::Point dPoint{ qVector.x(), qVector.y() };
			auto fpt = _points.insert(std::make_pair(dPoint, p2t::Point(dPoint._x, dPoint._y)));
			floatPath.emplace_back(&fpt.first->second);
		}
	}
	auto inserted = _nodeFloatCache.try_emplace(&node, std::move(floatPath));
	return inserted.first->second;
}
