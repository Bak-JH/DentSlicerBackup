#include "PolytreeCDT.h"
#include <unordered_set>
#include "../polyclipping.h"
using namespace ClipperLib;
using namespace Hix::Polyclipping;
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

std::unordered_map<PolyNode*, std::vector<PolytreeCDT::Triangle>> triangluateBFS(
	const PolyTree& tree, std::unordered_map<ClipperLib::PolyNode*, std::vector<p2t::Point*>>& floatPathMap
	)
{
	std::unordered_map<PolyNode*, std::vector<PolytreeCDT::Triangle>> result;

	// find solids
	std::unordered_set<PolyNode*> solids;
	std::deque<PolyNode*> q;
	auto firstSolids = tree.Childs;
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
		p2t::CDT cdt(floatPathMap[eachSolid]);
		//add holes
		for (auto hole : eachSolid->Childs)
		{
			cdt.AddHole(floatPathMap[hole]);
		}
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
	}

	return result;
}


std::unordered_map<PolyNode*, std::vector<PolytreeCDT::Triangle>> Hix::Polyclipping::PolytreeCDT::triangulate()
{
	if (_floatIntMap)
	{
		toFloatPtsWithMap();
	}
	else
	{
		toFloatPts();
	}
	return triangluateBFS(*_tree, _nodeFloatPtMap);
}


PolytreeCDT::~PolytreeCDT()
{
	for (auto& pair : _nodeFloatPtMap)
	{
		for (auto& pt : pair.second)
		{
			delete pt;
		}
	}
}

void PolytreeCDT::toFloatPts()
{
	auto curr = _tree->GetFirst();
	while (curr)
	{
		std::vector<p2t::Point*> floatPath;
		floatPath.reserve(curr->Contour.size());
		for (auto& point : curr->Contour)
		{
			floatPath.emplace_back(toDoublePtHeap(point));
		}
		_nodeFloatPtMap[curr] = floatPath;
		curr = curr->GetNext();
	}
}

void PolytreeCDT::toFloatPtsWithMap()
{
	auto curr = _tree->GetFirst();
	while (curr)
	{
		std::vector<p2t::Point*> floatPath;
		floatPath.reserve(curr->Contour.size());
		for (auto& point : curr->Contour)
		{
			auto origFloatPt = _floatIntMap->find(point);
			if (origFloatPt == _floatIntMap->end())
			{
				//no matching original float point detected
				floatPath.emplace_back(toDoublePtHeap(point));
			}
			else
			{
				auto qVector = origFloatPt->second;
				floatPath.emplace_back(new p2t::Point(qVector.x(), qVector.y()));
			}
		}
		_nodeFloatPtMap[curr] = floatPath;
		curr = curr->GetNext();
	}
}
