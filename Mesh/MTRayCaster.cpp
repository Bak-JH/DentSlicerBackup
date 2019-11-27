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

	QVector3D v0 = _accelerator->getWorldPos(mvs[0]);
	QVector3D v1 = _accelerator->getWorldPos(mvs[1]);
	QVector3D v2 = _accelerator->getWorldPos(mvs[2]);
	QVector3D v0v1 = v1 - v0;
	QVector3D v0v2 = v2 - v0;
	QVector3D pvec = QVector3D::crossProduct(rayDirection, v0v2);
	float det = v0v1.dotProduct(v0v1, pvec);
	bool isBackside = false;
	// if the determinant is negative the triangle is backfacing
	// if the determinant is close to 0, the ray misses the triangle
	if (det < EPSILON)
		isBackside = true;

	// ray and triangle are parallel if det is close to 0
	if (fabs(det) < EPSILON)
		return hit;
	float invDet = 1 / det;

	QVector3D tvec = rayOrigin - v0;
	auto u = QVector3D::dotProduct(tvec, pvec) * invDet;
	if (u < 0 || u > 1) 
		return hit;

	QVector3D qvec = QVector3D::crossProduct(tvec, v0v1);
	auto v = QVector3D::dotProduct(rayDirection, qvec) * invDet;
	if (v < 0 || u + v > 1) 
		return hit;

	auto t = QVector3D::dotProduct(v0v2, qvec) * invDet;
	hit.intersection = rayOrigin + rayDirection * t;
	if (isBackside)
		hit.type = HitType::BackSide;
	else
		hit.type = HitType::FrontSide;

	return hit;
}


//
//RayHit Hix::Engine3D::MTRayCaster::rayIntersectTri(const QVector3D& rayOrigin, const QVector3D& rayDirection, const FaceConstItr& tri)
//{
//	RayHit hit;
//	hit.type = HitType::Miss;
//	constexpr float EPSILON = std::numeric_limits<float>::epsilon();
//	auto mvs = tri.meshVertices();
//
//	QVector3D vertex0 = _accelerator->getWorldPos(mvs[0]);
//	QVector3D vertex1 = _accelerator->getWorldPos(mvs[1]);
//	QVector3D vertex2 = _accelerator->getWorldPos(mvs[2]);
//	QVector3D edge1, edge2, h, s, q;
//	float a, f, u, v;
//	edge1 = vertex1 - vertex0;
//	edge2 = vertex2 - vertex0;
//	h = QVector3D::crossProduct(rayDirection, edge2);
//	a = QVector3D::dotProduct(edge1, h);
//	if (a > -EPSILON && a < EPSILON)
//		return hit;    // This ray is parallel to this triangle.
//	f = 1.0 / a;
//	s = rayOrigin - vertex0;
//	u = f * QVector3D::dotProduct(s, h);
//
//	if (u < 0.0 || u > 1.0)
//		return hit;
//	q = QVector3D::crossProduct(s, edge1);
//	v = f * QVector3D::dotProduct(rayDirection, q);
//	if (v < 0.0 || u + v > 1.0)
//		return hit;
//	// At this stage we can compute t to find out where the intersection point is on the line.
//	float t = f * QVector3D::dotProduct(edge2, q);
//	if (t > EPSILON&& t < 1 / EPSILON) // ray intersection
//	{
//		hit.intersection = rayOrigin + rayDirection * t;
//		return hit;
//	}
//	else // This means that there is a line intersection but not a ray intersection.
//		return hit;
//}
