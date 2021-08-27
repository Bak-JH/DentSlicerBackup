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
	constexpr float approx = 0.0001f;
	auto mvs = tri.meshVertices();

	QVector3D v0 = _accelerator->getCachedPos(mvs[0]);
	QVector3D v1 = _accelerator->getCachedPos(mvs[1]);
	QVector3D v2 = _accelerator->getCachedPos(mvs[2]);

	QVector3D v0v1 = v1 - v0;
	QVector3D v0v2 = v2 - v0;

	QVector3D h = QVector3D::crossProduct(rayDirection, v0v2);
	float determinant = QVector3D::dotProduct(v0v1, h);
	bool isBackside = false;

	// Negative determinant would indicate back facing triangle
	if (determinant < 0.0f)
		isBackside = true;

	// Very small determinant means the ray is almost parallel with the triangle plane.
	if (determinant > -approx && determinant < approx)
	{
		hit.type = HitType::Degenerate;
		return hit;
	}

	float inverse_determinant = 1.0 / determinant;

	QVector3D origins_diff_vector = rayOrigin - v0;
	float u = QVector3D::dotProduct(origins_diff_vector, h) * inverse_determinant;

	// check float errors
	if ((std::abs(u) <= approx && std::abs(u) > 0.0f) ||
		(std::abs(u) - 1.0f <= approx && std::abs(u) > 1.0f))
	{
		hit.type = HitType::Degenerate;
		return hit;
	}

	// Check the u-barycentric coordinate for validity to save further expensive calculations
	if (u < 0.0 || u > 1.0)
	{
		return hit;
	}

	QVector3D q = QVector3D::crossProduct(origins_diff_vector, v0v1);
	float v = inverse_determinant * QVector3D::dotProduct(rayDirection, q);


	// check float errors
	if ((std::abs(v) <= approx && std::abs(v) > 0.0f) ||
		(std::abs((u + v) - 1.0f) <= approx && (u + v) != 1.0f))
	{
		hit.type = HitType::Degenerate;
		return hit;
	}


	// Check the v-barycentric coordinate for validity to save further expensive calculations
	if (v < 0.0 || u + v > 1.0)
	{
		return hit;
	}

	// At this stage we can compute t to find out where the intersection point is on the line
	float t = inverse_determinant * QVector3D::dotProduct(v0v2, q);

	if (t > approx)
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
