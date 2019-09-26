#include "ContourBuilder.h"

using namespace Hix::Slicer;
using namespace Hix::Engine3D;
using namespace ClipperLib;


inline void rotateCW90(QVector3D& vec)
{
	//(-y,x)
	auto tmp = vec.x();
	vec.setX(vec.y());
	vec.setY(-1.0f * tmp);
}
inline void rotateCW90(QVector2D& vec)
{
	//(-y,x)
	auto tmp = vec.x();
	vec.setX(vec.y());
	vec.setY(-1.0f * tmp);
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
		outFloatPath.reserve(segments.size() + 1);

		outFloatPath.emplace_back(segments.front().from);
		for (auto& each : segments)
		{
			outFloatPath.emplace_back(each.to);
		}
		path = Hix::Polyclipping::toCLPath(outFloatPath);
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
#ifdef _STRICT_SLICER
	if (to() != appended.from())
	{
		throw std::runtime_error("trying to append two disjoint contours");
	}
#endif
	for (auto each : appended.segments)
	{
		addNext(each);
	}
}


ContourSegment::FlipResult ContourSegment::calcNormalAndFlip()
{

	//determine direction
	QVector3D faceNormal = face->fn;
	faceNormal.setZ(0.0f);
	faceNormal.normalize();

	QVector2D ABNormal = to - from;
	rotateCW90(ABNormal);
	ABNormal.normalize();

	QVector2D BANormal = from - to;
	rotateCW90(BANormal);
	BANormal.normalize();

	//face normal projected over z-plane should still be normal for AB/BA vector.
	//Now determine which vector direction is correct by comparing CCW90 or CW90 normals to projected Face normal
	auto ABDiff = (ABNormal - faceNormal).lengthSquared();
	auto BADiff = (BANormal - faceNormal).lengthSquared();


	//since projected face normal is still a normal for the AB/BA vector
	//ABNormal == faceNormal or BANormal == faceNormal
	//hence minimum of those two diffs should be very close to be zero
	auto smallestDiff = std::min(ABDiff, BADiff);

	if (smallestDiff > 0.01f)
	{
		return FlipResult::UnknownDirection;
	}

	if (ABDiff < BADiff)
	{
		//AB is correct direction
		normal = ABNormal;
		return FlipResult::NotFlipped;
	}
	else
	{
		//BA is correct direction
		auto tmp = to;
		to = from;
		from = tmp;
		normal = BANormal;
		return FlipResult::Flipped;
	}
}
void ContourSegment::flip()
{
	auto tmp = to;
	to = from;
	from = tmp;
}


ContourBuilder::ContourBuilder(const Mesh* mesh, std::unordered_set<FaceConstItr>& intersectingFaces, float z)
	:_mesh(mesh), _plane(z), _intersectList(intersectingFaces)
{
}


QVector2D ContourBuilder::midPoint2D(VertexConstItr vtxA0, VertexConstItr vtxA1)
{
	QVector2D result;
	//A0.z > A1.z
	if (vtxA0->position.z() < vtxA1->position.z())
	{
		std::swap(vtxA0, vtxA1);
	}
	auto fullEdge = std::make_pair(vtxA0, vtxA1);
	auto preCalc = _midPtLUT.find(fullEdge);
	if (preCalc == _midPtLUT.end())
	{
		float x, y, zRatio;
		zRatio = ((_plane - vtxA0->position.z()) / (vtxA1->position.z() - vtxA0->position.z()));
		x = (vtxA1->position.x() - vtxA0->position.x()) * zRatio
			+ vtxA0->position.x();
		y = (vtxA1->position.y() - vtxA0->position.y()) * zRatio
			+ vtxA0->position.y();
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
	auto mfVertices = mf->meshVertices();
	for (int i = 0; i < 3; i++) {
		if (mfVertices[i]->position.z() > _plane) {
			upper.push_back(mfVertices[i]);
		}
		else if (mfVertices[i]->position.z() == _plane) {
			middle.push_back(mfVertices[i]);
		}
		else
			lower.push_back(mfVertices[i]);
	}
	std::vector<VertexConstItr> majority;
	std::vector<VertexConstItr> minority;
	//two edges intersect
	if (middle.size() == 0)
	{
		if (upper.size() == 2 && lower.size() == 1) {
			majority = upper;
			minority = lower;
		}
		else {
			majority = lower;
			minority = upper;
		}
		auto a = midPoint2D(majority[0], minority[0]);
		auto b = midPoint2D(majority[1], minority[0]);
		segment.from = a;
		segment.to = b;

	}
	else {
		//1 edge interesecting, 1 vertice on the plane
		if (upper.size() == 1 && lower.size() == 1 && middle.size() == 1) {
			auto a = midPoint2D(upper[0], lower[0]);
			segment.from = a;
			segment.to = QVector2D(middle[0]->position.x(), middle[0]->position.y());
		}
		else if (middle.size() == 2) {
			segment.from = QVector2D(middle[0]->position.x(), middle[0]->position.y());
			segment.to = QVector2D(middle[1]->position.x(), middle[1]->position.y());

		}

		//face == plane
	}

	ContourSegment::FlipResult flipResult;
	if (segment.from == segment.to)
	{
		return;
	}
	else
	{
		//hint needs to be in correct direction
		flipResult = segment.calcNormalAndFlip();
	}
	//insert to container
	_segments[mf] = segment;
	auto& newSeg = _segments[mf];

	//segment is too small and direction cannot be determinied
	if (flipResult == ContourSegment::FlipResult::UnknownDirection)
	{
		_unknownHash.insert(std::make_pair(newSeg.from, &newSeg));
		_unknownHash.insert(std::make_pair(newSeg.to, &newSeg));
	}
	else
	{
		_fromHash.insert(std::make_pair(newSeg.from, &newSeg));
		_toHash.insert(std::make_pair(newSeg.to, &newSeg));
	}


}

std::vector<Contour> ContourBuilder::buildContours()
{
	auto faces = _mesh->getFaces();
	std::vector<Contour> contours;
	_incompleteContours.clear();
	_unexplored = _intersectList;

	//create segments
	for (const auto& each : _intersectList)
	{
		buildSegment(each);
	}
	//remove segments of unknown direction from stating segment candidates.
	for (auto each : _unknownSegs)
	{
		_unexplored.erase(each->face);
	}

	while (!_unexplored.empty())
	{
		Contour currContour;
		//add first segment, pop it from unexplored
		ContourSegment& firstSeg = _segments[*_unexplored.begin()];
		currContour.addNext(firstSeg);
		_unexplored.erase(_unexplored.begin());


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
		auto closedContours = joinOrCloseIncompleteContours();
		for (auto each : closedContours)
		{
			if (isArea(*each))
			{
				contours.push_back(*each);
				qDebug() << "repaired contour";
			}
		}

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
	const std::unordered_multimap<QVector2D, Contour*>& map)
{
	//std::unordered_map<Contour*, Contour*> traverseMap;
	std::deque<Contour*> path;
	std::unordered_set<Contour*> explored;
	bool success = false;
	std::vector<Contour*> s;
	s.push_back(start);
	Contour* current;
	while (!s.empty())
	{
		current = s.back();
		s.pop_back();

		explored.insert(current);
		path.push_back(current);
		if (current->to() == start->from())
		{
			success = true;
			break;
		}
		else
		{
			auto childrenRange = map.equal_range(current->to());
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

	if (success && path.size() > 1)
	{

		//remove start contour form rPath
		path.pop_front();
		//append paths
		for (auto rItr = path.cbegin(); rItr != path.cend(); ++rItr)
		{
			start->append(**rItr);
			unusedContours.erase(*rItr);
		}
		//usually this happens when contour is made of one very small segment
		if (!start->isClosed())
		{
			return false;
		}
	}
	return success;


}

std::unordered_set<Contour*> ContourBuilder::joinOrCloseIncompleteContours()
{
	std::unordered_set<Contour*> remainingContours;
	std::unordered_set<Contour*> unjoinableContours;
	std::unordered_set<Contour*> closedContours;

	std::unordered_multimap<QVector2D, Contour*> map;
	for (auto& each : _incompleteContours)
	{
		IntPoint from = Hix::Polyclipping::toInt2DPt(each.from());
		IntPoint to = Hix::Polyclipping::toInt2DPt(each.to());
		each.from() = Hix::Polyclipping::toFloatPt(from);
		each.to() = Hix::Polyclipping::toFloatPt(to);
		if (from == to && each.dist() < 2)
			continue;
		remainingContours.insert(&each);
		closedContours.insert(&each);
		map.insert(std::make_pair(each.from(), &each));

	}
	while (!remainingContours.empty())
	{
		auto current = *remainingContours.begin();
		remainingContours.erase(current);
		bool stitchSuccess = DFSIncompleteContour(remainingContours, current, map);
		if (!stitchSuccess)
		{
			unjoinableContours.insert(current);
		}
	}
	//attempt to close unjoinable contours
	size_t i = 0;
	for (auto& each : unjoinableContours)
	{
		if (!each->tryClose())
		{
			++i;
			closedContours.erase(each);
		}
	}
	if (i != 0)
	{
		qDebug() << "unfixable contours";
	}
	return closedContours;
}




bool ContourBuilder::buildContour(Contour& current, bool reverse)
{
	std::unordered_multimap<QVector2D, ContourSegment*>* map;
	std::unordered_multimap<QVector2D, ContourSegment*>* oppMap;

	QVector2D from;
	if (reverse)
	{
		map = &_toHash;
		oppMap = &_fromHash;
		from = current.from();
	}
	else
	{
		map = &_fromHash;
		oppMap = &_toHash;
		from = current.to();
	}
	auto oppRange = oppMap->equal_range(from);
	auto range = map->equal_range(from);
    //range is of size 1
	auto nextItr = range.first;
	bool hasAny = range.first != range.second && oppRange.first != oppRange.second;
	bool hasOnlyOne = hasAny && (++range.first == range.second && ++oppRange.first == oppRange.second);
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


