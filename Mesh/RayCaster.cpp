#include "RayCaster.h"
#
using namespace  Hix;
using namespace Hix::Engine3D;
Hix::Engine3D::RayCaster::~RayCaster()
{
}
void Hix::Engine3D::RayCaster::addAccelerator(RayAccelerator* accelerator)
{
	_accelerator.reset(accelerator);
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
		if(result.type != HitType::Miss)
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
		rayHits.push_back(rayIntersectTri(rayFrom, rayDirection, each));
	}
	return rayHits;
}

