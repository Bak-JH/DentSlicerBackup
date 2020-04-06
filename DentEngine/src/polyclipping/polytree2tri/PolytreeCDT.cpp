#include "PolytreeCDT.h"
#include <unordered_set>
#include <qdebug.h>
#include "../../SlicerDebugInfoExport.h"
#include "../../../../application/ApplicationManager.h"
#include <array>
using namespace ClipperLib;
using namespace Hix::Polyclipping;
using namespace Hix::Slicer::Debug;



namespace std
{
	template<>
	struct hash<std::array<size_t, 2>> {
		std::size_t operator() (const std::array<size_t, 2>& key) const {
			return key[0] ^ key[1];
		}
	};
}



namespace Hix
{
	namespace Polyclipping
	{
		//implementation triangulate single solid and it's holes(so depth of only 2), we call this depth solid-hole pair depth, or just pair
		class CDTImpl
		{
		public:
			virtual ~CDTImpl();
			virtual std::vector<Triangle> pairTriangulate(const ClipperLib::PolyNode& solid) = 0;
		};
		class CDTImplPoly2Tri : public CDTImpl
		{
		public:
			CDTImplPoly2Tri(const std::unordered_map<ClipperLib::IntPoint, QVector2D>* map);
			virtual ~CDTImplPoly2Tri();
			std::vector<Triangle> pairTriangulate(const ClipperLib::PolyNode& solid)override;
		private:
			std::vector<p2t::Point*>& toFloatPts(const ClipperLib::PolyNode& node);
			void toFloatPtsImpl(const ClipperLib::IntPoint& point, std::vector<p2t::Point*>& floatPath);
			void toFloatPtsWithMap(const ClipperLib::IntPoint& point, std::vector<p2t::Point*>& floatPath);
			//points MUST BE UNIQUE!
			//this is map because p2t::point must be non-const
			std::unordered_map<CDT::V2d<double>, p2t::Point> _points;
			//this needs to be maintained as it owns the triangles after triangulation
			std::unordered_map<const ClipperLib::PolyNode*, std::vector<p2t::Point*>> _nodeFloatCache;

			const std::unordered_map<ClipperLib::IntPoint, QVector2D>* _floatIntMap = nullptr;
			RandomGen _random;

		};

		class CDTImplAG : public CDTImpl
		{
		public:
			CDTImplAG(const std::unordered_map<ClipperLib::IntPoint, QVector2D>* map);
			virtual ~CDTImplAG();
			std::vector<Triangle> pairTriangulate(const ClipperLib::PolyNode& solid)override;
		private:
			//returns index of inserted vtx
			size_t insertVtx(const CDT::V2d<double>& pt);
			CDT::V2d<double> dVtx(const ClipperLib::IntPoint& pt);
			CDT::V2d<double> dVtxWithMap(const ClipperLib::IntPoint& pt);

			void insertEdge(const ClipperLib::IntPoint& from, const ClipperLib::IntPoint& to);
			void insertContour(const ClipperLib::Path& contour);
			typedef  CDT::V2d<double> (CDTImplAG::* VtxFnPtr)(const ClipperLib::IntPoint& pt);  // Okay Bjarne!
			VtxFnPtr _addVtxFn;
			const std::unordered_map<ClipperLib::IntPoint, QVector2D>* _floatIntMap = nullptr;
			
			//point and index pair
			size_t _currIdx = 0;
			std::unordered_map< CDT::V2d<double>, size_t> _points;
			std::unordered_set<std::array<size_t,2>> _edges;
		};



	}

}





PolytreeCDT::PolytreeCDT(const ClipperLib::PolyTree* polytree): _tree(polytree)
{
}

PolytreeCDT::PolytreeCDT(const std::unordered_map<ClipperLib::IntPoint, QVector2D>* map, const ClipperLib::PolyTree* polytree):
	_tree(polytree), _floatIntMap(map)
{
}

Triangle convertTrig(p2t::Triangle* orig)
{
	Triangle trig;
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


std::unordered_map<PolyNode*, std::vector<Triangle>> Hix::Polyclipping::PolytreeCDT::triangulate()
{


	double pixelArea = Hix::Application::ApplicationManager::getInstance().settings().printerSetting.pixelSizeX() *
		Hix::Application::ApplicationManager::getInstance().settings().printerSetting.pixelSizeY();
	std::unordered_map<PolyNode*, std::vector<Triangle>> result;

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
		CDTImplPoly2Tri impl(_floatIntMap);
		result[eachSolid] = std::move(impl.pairTriangulate(*eachSolid));
	}

	return result;
}


PolytreeCDT::~PolytreeCDT()
{
}




//IMPLS


Hix::Polyclipping::CDTImpl::~CDTImpl()
{
}


Hix::Polyclipping::CDTImplPoly2Tri::CDTImplPoly2Tri(const std::unordered_map<ClipperLib::IntPoint, QVector2D>* map) : _random(1), _floatIntMap(map)
{
}

Hix::Polyclipping::CDTImplPoly2Tri::~CDTImplPoly2Tri()
{
}


std::vector<CDT::V2d<double>> debugPath(std::vector<p2t::Point*>& node)
{
	std::vector<CDT::V2d<double>> res;
	res.reserve(node.size());
	for (auto e : node)
	{
		res.push_back({ e->x * 51200 + 1000000, e->y * 51200 + 1000000 });
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
#ifdef _DEBUG
static size_t dSliceCnt = 0;
#endif

std::vector<Triangle> Hix::Polyclipping::CDTImplPoly2Tri::pairTriangulate(const ClipperLib::PolyNode& solid)
{
	double pixelArea = Hix::Application::ApplicationManager::getInstance().settings().printerSetting.pixelSizeX() *
		Hix::Application::ApplicationManager::getInstance().settings().printerSetting.pixelSizeY();
	std::vector<Triangle> result;
	if (tooSmallForTrig(solid, pixelArea))
		return result;

	//auto isSolidDupe = pathContainsDupe(toFloatPts(*eachSolid));
	auto solidF = toFloatPts(solid);
	if (solidF.empty())
		return result;
	p2t::CDT cdt(solidF);
	//add holes
	for (auto hole : solid.Childs)
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
		result.reserve(p2tTrigs.size());
		//convert to more robust value trigs
		for (auto& trig : p2tTrigs)
		{
			result.emplace_back(convertTrig(trig));
		}
		//stupid fucking code
		cdt.resetPoints();
	}
	catch (const std::runtime_error & error)
	{
#ifdef _DEBUG
		auto solidPath = debugPath(toFloatPts(solid));
		{
			SVGOut output("debug_svg" + std::to_string(dSliceCnt) + "solid", QSize(2000000, 2000000));
			QPen pen(QColor::fromRgb(200, 0, 0), 1, Qt::SolidLine);
			output.addPath(solidPath, pen, "solid");
		}

		size_t cnt = 0;
		for (auto hole : solid.Childs)
		{
			SVGOut output("debug_svg" + std::to_string(dSliceCnt) + "hole" + std::to_string(cnt), QSize(2000000, 2000000));
			std::string name = "hole" + std::to_string(cnt);
			++cnt;
			//auto isHoleDupe = pathContainsDupe(toFloatPts(*hole));
			QPen holePen(output.randColor(), 1, Qt::SolidLine);
			output.addPath(debugPath(toFloatPts(*hole)), holePen, "");
		}
#endif
	}
	return result;
}

std::vector<p2t::Point*>& Hix::Polyclipping::CDTImplPoly2Tri::toFloatPts(const ClipperLib::PolyNode& node)
{
	auto cached = _nodeFloatCache.find(&node);
	if (cached != _nodeFloatCache.end())
		return cached->second;

	std::vector<p2t::Point*> floatPath;
	floatPath.reserve(node.Contour.size());
	//pathContainsDupe(node.Contour);
	auto cleanedContour = node.Contour;
	//ClipperLib::CleanPolygon(cleanedContour);
	//if (cleanedContour != node.Contour)
	//{
	//	qDebug() << "cleaning worked";
	//}
	for (auto& point : cleanedContour)
	{
		if (_floatIntMap)
		{
			toFloatPtsWithMap(point, floatPath);
		}
		else
		{
			toFloatPtsImpl(point, floatPath);
		}

	}
	auto inserted = _nodeFloatCache.try_emplace(&node, std::move(floatPath));
	return inserted.first->second;


}


//inline void boxDither(size_t count, Hix::Polyclipping::Point& pt)
//{
//	auto ditherCase = count % 8;
//	switch (ditherCase)
//	{
//	case 0:
//		pt._x += std::numeric_limits<double>::epsilon() * (double)count;
//		pt._y += std::numeric_limits<double>::epsilon() * (double)count;
//		break;
//	case 1:
//
//		break;
//	case 2:
//		break;
//	case 3:
//		break;
//	case 4:
//		break;
//	case 5:
//		break;
//	case 6:
//		break;
//	case 7:
//		break;
//	default:
//		break;
//	}
//}

//constexpr double MIN_PIX_AREA = 57.9261875148;
//constexpr double MIN_PIX_AREA = 2;
inline void randomDither(CDT::V2d<double>& pt, RandomGen& rand)
{
	qDebug() << "before random dither: x=" << pt.x << "y=" << pt.y;
	constexpr int64_t MAX_MULT = 20;
	constexpr int64_t MIN_MULT = -20;

	double xDither = std::numeric_limits<double>::epsilon() * (double)rand.getInt(MIN_MULT, MAX_MULT);
	double yDither = std::numeric_limits<double>::epsilon() * (double)rand.getInt(MIN_MULT, MAX_MULT);
	pt.x += xDither;
	pt.y += yDither;
	qDebug() << "after random dither: x=" << pt.x << "y=" << pt.y;

}


void CDTImplPoly2Tri::toFloatPtsImpl(const ClipperLib::IntPoint& point, std::vector<p2t::Point*>& floatPath)
{

	auto dPoint = toDPt(point);
	auto fpt = _points.insert(std::make_pair(dPoint, p2t::Point(dPoint.x, dPoint.y)));
	while (!fpt.second)
	{
		randomDither(dPoint, _random);
		fpt = _points.insert(std::make_pair(dPoint, p2t::Point(dPoint.x, dPoint.y)));
	}
	floatPath.emplace_back(&fpt.first->second);

}

void CDTImplPoly2Tri::toFloatPtsWithMap(const ClipperLib::IntPoint& point, std::vector<p2t::Point*>& floatPath)
{
	auto origFloatPt = _floatIntMap->find(point);
	CDT::V2d<double> dPoint;
	if (origFloatPt == _floatIntMap->end())
	{
		//no matching original float point detected
		auto dPoint = toDPt(point);
	}
	else
	{
		auto qVector = origFloatPt->second;
		dPoint = { qVector.x(), qVector.y() };
	}
	auto fpt = _points.insert(std::make_pair(dPoint, p2t::Point(dPoint.x, dPoint.y)));
	while (!fpt.second)
	{
		randomDither(dPoint, _random);
		fpt = _points.insert(std::make_pair(dPoint, p2t::Point(dPoint.x, dPoint.y)));
	}
	floatPath.emplace_back(&fpt.first->second);

}



// aG CDT impl

CDTImplAG::CDTImplAG(const std::unordered_map<ClipperLib::IntPoint, QVector2D>* map): _floatIntMap(map)
{
	//funnily enough, branch prediciton works with these function pointers
	if (_floatIntMap)
	{
		_addVtxFn = &CDTImplAG::dVtxWithMap;
	}
	else
	{
		_addVtxFn = &CDTImplAG::dVtx;
	}
}

CDTImplAG::~CDTImplAG()
{
}


std::vector<Triangle> CDTImplAG::pairTriangulate(const ClipperLib::PolyNode& solid)
{
	std::vector<Triangle> result;
	//Artem-Ogre CDT auto detects holes, so all contours are treated same

	insertContour(solid.Contour);
	//add holes
	for (auto hole : solid.Childs)
	{
		insertContour(hole->Contour);
	}
	try
	{
		CDT::Triangulation<double> cdt = CDT::Triangulation<double>(CDT::FindingClosestPoint::BoostRTree);
		std::vector<CDT::V2d<double>> vtcs(_points.size());
		//unordered map of pt and indices to vector with matching order
		for (auto& pt : _points)
		{
			vtcs[pt.second] = pt.first;
		}
		cdt.insertVertices(vtcs);

		std::vector<CDT::Edge> edges;
		edges.reserve(_edges.size());
		for (auto& e : _edges)
		{
			auto t = e[0];
			edges.emplace_back(CDT::Edge(e[0], e[1]));
		}

		cdt.tryInsertEdges(edges);
		cdt.eraseOuterTrianglesAndHoles();
	}
	catch (const std::runtime_error & error)
	{
#ifdef _DEBUG
		//auto solidPath = debugPath(toFloatPts(solid));
		//{
		//	SVGOut output("debug_svg" + std::to_string(dSliceCnt) + "solid", QSize(2000000, 2000000));
		//	QPen pen(QColor::fromRgb(200, 0, 0), 1, Qt::SolidLine);
		//	output.addPath(solidPath, pen, "solid");
		//}

		//size_t cnt = 0;
		//for (auto hole : solid.Childs)
		//{
		//	SVGOut output("debug_svg" + std::to_string(dSliceCnt) + "hole" + std::to_string(cnt), QSize(2000000, 2000000));
		//	std::string name = "hole" + std::to_string(cnt);
		//	++cnt;
		//	//auto isHoleDupe = pathContainsDupe(toFloatPts(*hole));
		//	QPen holePen(output.randColor(), 1, Qt::SolidLine);
		//	output.addPath(debugPath(toFloatPts(*hole)), holePen, "");
		}
#endif
	}
	return result;


}


CDT::V2d<double> Hix::Polyclipping::CDTImplAG::dVtx(const ClipperLib::IntPoint& pt)
{
	return toDPt(pt);
}

CDT::V2d<double> Hix::Polyclipping::CDTImplAG::dVtxWithMap(const ClipperLib::IntPoint& pt)
{
	auto origFloatPt = _floatIntMap->find(pt);
	if (origFloatPt == _floatIntMap->end())
	{
		//no matching original float point detected
		return toDPt(pt);
	}
	else
	{
		auto qVector = origFloatPt->second;
		return { qVector.x(), qVector.y() };
	}
}


size_t Hix::Polyclipping::CDTImplAG::insertVtx(const CDT::V2d<double>& pt)
{
	auto result = _points.insert(std::make_pair(pt, _currIdx));
	if (result.second)
		++_currIdx;
	return result.first->second;
}


void Hix::Polyclipping::CDTImplAG::insertEdge(const ClipperLib::IntPoint& from, const ClipperLib::IntPoint& to)
{
	auto fIndex = insertVtx((this->*_addVtxFn)(from));
	auto tIndex = insertVtx((this->*_addVtxFn)(to));
	if (fIndex > tIndex)
	{
		std::swap(fIndex, tIndex);
	}
	_edges.insert({ fIndex, tIndex });
}

void Hix::Polyclipping::CDTImplAG::insertContour(const ClipperLib::Path& contour)
{
	auto lastItr = contour.cend() - 1;
	for (auto curr = contour.cbegin(); curr != lastItr; ++curr)
	{
		insertEdge(*curr, *(curr + 1));
	}
	//since clipperLib have open contour ie) last edge is not counted
	insertEdge(contour.back(), contour.front());
}
