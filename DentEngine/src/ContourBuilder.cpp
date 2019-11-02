#include "ContourBuilder.h"
#include "../../feature/Shapes2D.h"
#include "SlicerDebug.h"
#include "SlicerDebugInfoExport.h"
#include "configuration.h"
using namespace Hix::Slicer;
using namespace Hix::Engine3D;
using namespace ClipperLib;


//inline void rotateCW90(QVector3D& vec)
//{
//	//(y,-x)
//	auto tmp = vec.x();
//	vec.setX(vec.y());
//	vec.setY(-tmp);
//}
inline void rotateCW90(QVector2D& vec)
{
	//(y,-x)
	auto tmp = vec.x();
	vec.setX(vec.y());
	vec.setY(-tmp);
}
inline void rotateCCW90(QVector2D& vec)
{
	//(-y,x)
	auto tmp = vec.x();
	vec.setX(-vec.y());
	vec.setY(tmp);
}

bool Contour::isClosed()const
{
	if (segments.front().from == segments.back().to)
	{
		return true;
	}
	return false;
}


bool Contour::tryClose()
{
#ifdef _STRICT_SLICER
	if (isClosed())
	{
		throw std::runtime_error("force close on already closed contour");
	}
#endif
	if (segments.size() < 2)
		return false;
	float totalDist = dist();
	float gap = segments.front().from.distanceToPoint(segments.back().to);
	if (gap < std::numeric_limits<float>::epsilon() || gap < totalDist / 5)
	{
		ContourSegment closer;
		closer.from = segments.back().to;
		closer.to = segments.front().from;
		addNext(closer);
#ifdef _STRICT_SLICER
		if (!isClosed())
		{
			throw std::runtime_error("force close failed");
		}
#endif
		return true;
	}
	return false;
}

void Contour::addNext(const ContourSegment& seg)
{
#ifdef _STRICT_SLICER
	//if (getDestination() != seg->from)
	//	throw std::runtime_error("mismatching segment added to contour");
#endif
	segments.push_back(seg);
	//checkBound(seg->to);
}
void Contour::addPrev(const ContourSegment& seg)
{
#ifdef _STRICT_SLICER
	//if (getDestination() != seg->from)
	//	throw std::runtime_error("mismatching segment added to contour");
#endif
	segments.push_front(seg);
	//checkBound(seg->to);
}

float Contour::dist()const
{
	float totalDist = 0;
	for (auto& each : segments)
	{
		totalDist += each.dist();
	}
	return totalDist;
}

QVector2D& Contour::from()
{
	return segments.front().from;
}
QVector2D& Contour::to()
{
	return segments.back().to;
}
Path Contour::toPath(std::vector<QVector2D>& outFloatPath)const
{
	Path path;
	if (!segments.empty())
	{
		outFloatPath.reserve(segments.size());

		for (auto& each : segments)
		{
			outFloatPath.emplace_back(each.to);
		}
		path = Hix::Polyclipping::toCLPath(outFloatPath);
	}

	return path;
}

ClipperLib::Path Hix::Slicer::Contour::toDebugPath() const
{
	Path path;
	if (!segments.empty())
	{
		std::vector<QVector2D> ptPath;
		ptPath.reserve(segments.size());

		for (auto& each : segments)
		{
			ptPath.emplace_back(each.to);
		}
		path = Hix::Polyclipping::toDebugPixelPath(ptPath);
	}

	return path;
}

Path Contour::toPath()const
{
	std::vector<QVector2D> tmp;
	return toPath(tmp);

}

void Contour::append(const Contour& appended)
{
	auto appendBeginIdx = segments.size();
	for (auto each : appended.segments)
	{
		addNext(each);
	}
	if (appendBeginIdx > 0)
		segments[appendBeginIdx].from = segments[appendBeginIdx - 1].to;
}


bool Hix::Slicer::Contour::isOutward()const
{
	return !Hix::Shapes2D::isClockwise(*this);
}




ContourBuilder::ContourBuilder(const Mesh* mesh, std::unordered_set<FaceConstItr>& intersectingFaces, float z)
	:_mesh(mesh), _plane(z), _intersectList(intersectingFaces)
{
}


QVector2D ContourBuilder::planeIntersectionPt(VertexConstItr vtxA0, VertexConstItr vtxA1)
{
	QVector2D result;
	//A0.z > A1.z
	auto a0pos = vtxA0.worldPosition();
	auto a1pos = vtxA1.worldPosition();
	if (a0pos.z() < a1pos.z())
	{
		std::swap(vtxA0, vtxA1);
	}
	auto fullEdge = std::make_pair(vtxA0, vtxA1);
	auto preCalc = _midPtLUT.find(fullEdge);
	if (preCalc == _midPtLUT.end())
	{
		float x, y, zRatio;
		zRatio = ((_plane - a0pos.z()) / (a1pos.z() - a0pos.z()));
		x = (a1pos.x() - a0pos.x()) * zRatio
			+ a0pos.x();
		y = (a1pos.y() - a0pos.y()) * zRatio
			+ a0pos.y();
		result = QVector2D(x, y);
		_midPtLUT[fullEdge] = result;
	}
	else
	{
		result = preCalc->second;
	}
	return result;
}



void ContourBuilder::buildSegment(const FaceConstItr& mf)
{
	ContourSegment segment;
	segment.face = mf;
	std::vector<VertexConstItr> upper;
	std::vector<VertexConstItr> middle;
	std::vector<VertexConstItr> lower;


	auto mfVertices = mf.meshVertices();
	for (int i = 0; i < 3; i++) {
		auto pos = mfVertices[i].worldPosition();
		if (pos.z() > _plane) {
			upper.push_back(mfVertices[i]);
		}
		else if (pos.z() == _plane) {
			middle.push_back(mfVertices[i]);
		}
		else
			lower.push_back(mfVertices[i]);
	}
	//two edges intersect
	if (middle.size() == 0)
	{
		std::vector<VertexConstItr> majority;
		std::vector<VertexConstItr> minority;
		if (upper.size() == 2 && lower.size() == 1) {
			majority = upper;
			minority = lower;
		}
		else {
			majority = lower;
			minority = upper;
		}
		//need to sort majority to have correct orientation
		if (!mf.isNextVtx(majority[0], majority[1]))
		{
			std::swap(majority[0], majority[1]);
		}
		//need to swap again if high pt is minority, ie) pyramid shape
		if (upper.size() == 1)
		{
			std::swap(majority[0], majority[1]);
		}
		segment.from = planeIntersectionPt(majority[1], minority[0]);
		segment.to = planeIntersectionPt(majority[0], minority[0]);
	}
	else {
		auto middle0pos = QVector2D(middle[0].worldPosition());
		//1 edge interesecting, 1 vertice on the plane
		if (upper.size() == 1 && lower.size() == 1 && middle.size() == 1) {
			auto edgeIntersect = planeIntersectionPt(upper[0], lower[0]);
			//need to check orientation if its HLM or MLH, H = High, L = Low, M = Middle
			if (mf.isNextVtx(upper[0], lower[0]))
			{
				segment.from = edgeIntersect;
				segment.to = middle0pos;
			}
			else
			{
				segment.from = middle0pos;
				segment.to = edgeIntersect;
			}

		}
		else if (middle.size() == 2) {
			auto middle1pos = QVector2D(middle[1].worldPosition());
			//also needs to check orientation
			if (!mf.isNextVtx(middle[0], middle[1]))
			{
				std::swap(middle0pos, middle1pos);
			}
			//for V shape, order is also reversed;
			if (!lower.empty())
			{
				std::swap(middle0pos, middle1pos);
			}
			segment.from = middle0pos;
			segment.to = middle1pos;
		}
	}

	if (segment.from == segment.to)
	{
		return;
	}

	//insert to container
	_segments[mf] = segment;
	auto& newSeg = _segments[mf];
	_unexplored.insert(mf);

	_fromHash.insert(std::make_pair(newSeg.from, &newSeg));
	_toHash.insert(std::make_pair(newSeg.to, &newSeg));

}

std::vector<Contour> ContourBuilder::buildContours()
{
	auto faces = _mesh->getFaces();
	std::vector<Contour> contours;
	_incompleteContours.clear();

	//create segments
	for (const auto& each : _intersectList)
	{
		buildSegment(each);
	}
	while (!_unexplored.empty())
	{
		Contour currContour;
		//add first segment, pop it from unexplored
		ContourSegment& firstSeg = _segments[*_unexplored.begin()];
		_unexplored.erase(firstSeg.face);
		currContour.addNext(firstSeg);
		//add rest
		uint8_t deadEndCnt = 0;
		while (deadEndCnt != 2)
		{
			if (!buildContour(currContour, deadEndCnt == 1))
			{
				++deadEndCnt;
			}
			//if contour is complete
			if (currContour.isClosed())
			{
				break;
			}
		}
		//if unclosed contour
		if (deadEndCnt == 2 || !isArea(currContour))
		{
			_incompleteContours.push_back(std::move(currContour));
		}
		else
		{
			contours.push_back(std::move(currContour));
		}


	}
	//link un-closed contours together if possible to minimize un-closed contour counts
	if (!_incompleteContours.empty())
	{
		auto repairedContours = joinOrCloseIncompleteContours();
		std::move(repairedContours.begin(), repairedContours.end(), std::back_inserter(contours));
	}
	//repair remaining un-closed contorus
	return contours;
}
std::vector<Contour> Hix::Slicer::ContourBuilder::flushIncompleteContours()
{
	return std::move(_incompleteContours);
}
bool ContourBuilder::isArea(const Contour& contour)
{
	if (contour.segments.size() > 2)
		return true;
	else
		return false;
}



bool DFSIncompleteContour(std::unordered_set<Contour*>& unusedContours, Contour* start,
	const std::unordered_multimap<IntPoint, Contour*>& fromMap, const std::unordered_map<Contour*, IntPoint>& toMap, Contour& out)
{
	//std::unordered_map<Contour*, Contour*> traverseMap;
	std::deque<Contour*> path;
	std::unordered_set<Contour*> explored;
	std::vector<Contour*> s;
	s.push_back(start);
	bool combineSuccess = false;
	IntPoint startFrom = Hix::Polyclipping::toInt2DPt(start->from());
	Contour* current;
	while (!s.empty())
	{
		current = s.back();
		s.pop_back();

		explored.insert(current);
		path.push_back(current);
		if (startFrom == toMap.at(current))
		{
			combineSuccess = true;
			break;
		}
		else
		{
			auto childrenRange = fromMap.equal_range(toMap.at(current));
			size_t added = 0;
			for (auto it = childrenRange.first; it != childrenRange.second; ++it) {
				if (explored.find(it->second) == explored.end())
				{
					s.push_back(it->second);
					++added;
				}
			}
			if (added == 0)
			{
				//dead end
				explored.erase(current);
				path.pop_back();
			}
		}
	}
	out = *start;
	if (combineSuccess && path.size() > 1)
	{
		//remove start contour form rPath
		path.pop_front();
		//append paths
		for (auto rItr = path.cbegin(); rItr != path.cend(); ++rItr)
		{
			out.append(**rItr);
			unusedContours.erase(*rItr);
		}
	}
	//usually this happens when contour is made of one very small segment
	if (!out.isClosed())
	{
		if (!out.tryClose())
		{
			return false;
		}
	}
	return ContourBuilder::isArea(out);
}

std::vector<Contour> ContourBuilder::joinOrCloseIncompleteContours()
{
	std::unordered_set<Contour*> remainingContours;
	std::unordered_set<Contour*> unjoinableContours;
	std::vector<Contour> closedContours;
	closedContours.reserve(_incompleteContours.size());
	//debug log
	qDebug() << "incomplete contours: " << _incompleteContours.size();

	std::unordered_multimap<IntPoint, Contour*> fromMap;
	std::unordered_map<Contour*, IntPoint> toMap;

	for (auto& each : _incompleteContours)
	{
		IntPoint from = Hix::Polyclipping::toInt2DPt(each.from());
		IntPoint to = Hix::Polyclipping::toInt2DPt(each.to());
		//each.from() = Hix::Polyclipping::toFloatPt(from);
		////each.to() = Hix::Polyclipping::toFloatPt(to);
		//if (from == to)
		//	continue;
		//auto fromToDist = each.from().distanceToPoint(each.to());
		if (each.dist() < Hix::Polyclipping::FLOAT_PT_RESOLUTION)
			continue;
		remainingContours.insert(&each);
		fromMap.insert(std::make_pair(from, &each));
		toMap.insert(std::make_pair(&each, to));
	}
	while (!remainingContours.empty())
	{
		auto current = *remainingContours.begin();
		remainingContours.erase(current);
		Contour result;
		bool stitchSuccess = DFSIncompleteContour(remainingContours, current, fromMap, toMap, result);
		if (!stitchSuccess)
		{
			unjoinableContours.insert(current);
		}
		else
		{
			closedContours.emplace_back(std::move(result));
		}
	}
	//remove repaired contours from incomplete contours
	auto itr = _incompleteContours.begin();
	while (itr != _incompleteContours.end())
	{
		if (unjoinableContours.find(&(*itr)) != unjoinableContours.end())
			itr = _incompleteContours.erase(itr);
		else
			++itr;
	}
	qDebug() << "repaired contours: " << closedContours.size();
	return closedContours;
}




bool ContourBuilder::buildContour(Contour& current, bool reverse)
{
	std::unordered_multimap<QVector2D, ContourSegment*>* map;

	QVector2D from;
	if (reverse)
	{
		map = &_toHash;
		from = current.from();
	}
	else
	{
		map = &_fromHash;
		from = current.to();
	}
	auto range = map->equal_range(from);
    //range is of size 1
	auto nextItr = range.first;
	bool hasAny = range.first != range.second;
	bool hasOnlyOne = hasAny && (++range.first == range.second);
    if (hasOnlyOne && _unexplored.find(nextItr->second->face) != _unexplored.end())
	{
		const ContourSegment& next = *nextItr->second;

        if (reverse)
        {
            current.addPrev(next);
        }
        else
        {
            current.addNext(next);
        }
        //mark explored
        _unexplored.erase(next.face);
        return true;
	}
	return false;
}



