#include "RayCaster.h"
#include "../feature/Shapes2D.h"
using namespace Hix;
using namespace Hix::Engine3D;
using namespace Hix::Shapes2D;
Hix::Engine3D::RayCaster::~RayCaster()
{
}
void Hix::Engine3D::RayCaster::addAccelerator(RayAccelerator* accelerator)
{
	_accelerator = accelerator;
}

RayHits Hix::Engine3D::RayCaster::rayIntersect(const QVector3D& rayOrigin, const QVector3D& rayEnd)
{
	QVector3D rayDirection(rayEnd - rayOrigin);
	rayDirection.normalize();
	RayHits rayHits;
	auto candidates = _accelerator->getRayCandidates(rayOrigin, rayEnd);
	for (auto& each : candidates)
	{
		auto result = rayIntersectTri(rayOrigin, rayDirection, each);
		if (result.type == HitType::Degenerate)
		{
			rayHits.clear();
			rayHits.push_back(result);
			return rayHits;
		}
		if(result.type != HitType::Miss)
			rayHits.emplace_back(result);
	}
	return rayHits;
}

RayHits Hix::Engine3D::RayCaster::rayIntersectWithoutDegen(const QVector3D& rayOrigin, const QVector3D& rayEnd)
{
	QVector3D rayDirection(rayEnd - rayOrigin);
	rayDirection.normalize();
	RayHits rayHits;
	auto candidates = _accelerator->getRayCandidates(rayOrigin, rayEnd);
	for (auto& each : candidates)
	{
		auto result = rayIntersectTriWIthoutDegen(rayOrigin, rayDirection, each);

		if (result.type != HitType::Miss)
			rayHits.emplace_back(result);
	}
	return rayHits;
}

RayHits Hix::Engine3D::RayCaster::rayIntersectDirection(const QVector3D& rayFrom, const QVector3D& rayDirection)
{
	//pre: rayDirection should be normalized
	RayHits rayHits;
	auto candidates = _accelerator->getRayCandidatesDirection(rayFrom, rayDirection);
	for (auto& each : candidates)
	{
		auto result = rayIntersectTri(rayFrom, rayDirection, each);
		if (result.type == HitType::Degenerate)
		{
			rayHits.clear();
			rayHits.push_back(result);
			return rayHits; 
		}
		if (result.type != HitType::Miss)
			rayHits.push_back(result);
	}
	return rayHits;
}

std::optional<RayHit> Hix::Engine3D::RayCaster::getFirstFront(RayHits& hits)
{
	RayHits onlyFronts;
	std::for_each(std::begin(hits), std::end(hits), [&onlyFronts](const RayHit& hit) {
		if (hit.type == HitType::FrontSide)
		{
			onlyFronts.emplace_back(hit);
		}
		});
	if (onlyFronts.empty())
		return {};
	auto min = std::min_element(std::begin(onlyFronts), std::end(onlyFronts), [](const RayHit& a, const RayHit& b) {
		return a.distance < b.distance;
		
		});
	return *min;

}



bool Hix::Engine3D::RayCaster::checkCSecInteresect(const QVector3D& origin, const QVector3D& end, const std::vector<QVector3D>& crossSec)
{
	std::array<QVector3D, 4> rectTop;
	std::array<QVector3D, 4> rectBot;
	RayHits hits;
	auto segCnt = crossSec.size();
	for (auto& pt : crossSec)
	{
		QVector3D top = origin + pt;
		QVector3D bot = end + pt;
		auto rayCastResults = rayIntersect(top, bot);
		if (!rayCastResults.empty())
			return false;
	}
	return true;
}


