#include "MTRayCaster.h"
#include <limits>

using namespace  Hix;
using namespace Hix::Engine3D;
Hix::Engine3D::MTRayCaster::MTRayCaster()
{
}

Hix::Engine3D::MTRayCaster::~MTRayCaster()
{
}


RayHit Hix::Engine3D::MTRayCaster::rayIntersectTri(const QVector3D& rayOrigin, const QVector3D& rayDirection, const FaceConstItr& tri)
{
	RayHit hit;
	hit.type = HitType::Miss;
	hit.face = tri;
	constexpr float EPSILON = std::numeric_limits<float>::epsilon();
	auto mvs = tri.meshVertices();

	QVector3D v0 = _accelerator->getCachedPos(mvs[0]);
	QVector3D v1 = _accelerator->getCachedPos(mvs[1]);
	QVector3D v2 = _accelerator->getCachedPos(mvs[2]);

	QVector3D edge1 = v1 - v0;
	QVector3D edge2 = v2 - v0;

	QVector3D h = QVector3D::crossProduct(rayDirection, edge2);
	float determinant = QVector3D::dotProduct(edge1, h);
	bool isBackside = false;

	// Negative determinant would indicate back facing triangle
	if (determinant < 0.0f)
		isBackside = true;

	// Very small determinant means the ray is almost parallel with the triangle plane.
	if (determinant > -EPSILON && determinant < EPSILON)
	{
		hit.type = HitType::Degenerate;
		return hit;
	}

	float inverse_determinant = 1.0 / determinant;

	QVector3D origins_diff_vector = rayOrigin - v0;
	float u = QVector3D::dotProduct(origins_diff_vector, h) * inverse_determinant;

	if ((std::abs(u) <= EPSILON && std::abs(u) > 0.0f) || (std::abs(std::abs(u) - 1.0f) <= EPSILON && std::abs(u) > 1.0f))
	{
		hit.type = HitType::Degenerate;
		return hit;
	}

	// Check the u-barycentric coordinate for validity to save further expensive calculations
	if (u < 0.0 || u > 1.0)
	{
		return hit;
	}

	QVector3D q = QVector3D::crossProduct(origins_diff_vector, edge1);
	float v = inverse_determinant * QVector3D::dotProduct(rayDirection, q);

	// Check the v-barycentric coordinate for validity to save further expensive calculations
	if ((std::abs(v) <= EPSILON && std::abs(v) > 0.0f) || (std::abs(std::abs(v) - 1.0f) <= EPSILON && std::abs(v) > 1.0f))
	{
		hit.type = HitType::Degenerate;
		return hit;
	}

	if (v < 0.0 || u + v > 1.0)
	{
		return hit;
	}

	// At this stage we can compute t to find out where the intersection point is on the line
	float t = inverse_determinant * QVector3D::dotProduct(edge2, q);

	if (t > EPSILON)
	{
		hit.distance = t;
		hit.intersection = rayOrigin + rayDirection * t;

		if (isBackside)
			hit.type = HitType::BackSide;
		else
			hit.type = HitType::FrontSide;

		return hit;
	}
	else
	{
		// This means that there is a line intersection but not a ray intersection
		return hit;
	}
}
