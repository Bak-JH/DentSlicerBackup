#pragma once
#include "../catch.hpp"

#include "Mesh/mesh.h"
#include "Mesh/RayAccelerator.h"
#include "Mesh/MTRayCaster.h"
#include "Mesh/BVH.h"
#include "Mesh/HixBVH.h"

#include <QVector2D>

namespace RayTest
{
	using namespace Hix;
	using namespace Hix::Engine3D;

	std::unique_ptr<Hix::Engine3D::RayCaster> _rayCaster;
	std::unique_ptr<Hix::Engine3D::RayAccelerator> _rayAccel;

	QVector3D getProj(QVector3D vecOrigin, QVector3D vecNormal)
	{
		return vecOrigin - 
				(QVector3D::dotProduct(vecOrigin, vecNormal) / vecNormal.lengthSquared() * vecNormal);
	}

	RayHit rayIntersectTri(const QVector3D& rayOrigin, const QVector3D& rayDirection, const FaceConstItr& tri)
	{
		RayHit hit;
		hit.type = HitType::Miss;
		hit.face = tri;
		constexpr float EPSILON = std::numeric_limits<float>::epsilon();
		auto mvs = tri.meshVertices();

		QVector3D v0 = _rayAccel->getCachedPos(mvs[0]);
		QVector3D v1 = _rayAccel->getCachedPos(mvs[1]);
		QVector3D v2 = _rayAccel->getCachedPos(mvs[2]);
		QVector3D v0v1 = v1 - v0;
		QVector3D v0v2 = v2 - v0;
		QVector3D v1v2 = v2 - v1;
		QVector3D pvec = QVector3D::crossProduct(rayDirection, v0v2);
		float det = v0v1.dotProduct(v0v1, pvec);

		bool isBackside = false;
		// if the determinant is negative the triangle is backfacing
		// if the determinant is close to 0, the ray misses the triangle
		if (det < EPSILON)
			isBackside = true;

		// ray and triangle are parallel if det is close to 0
		if (fabs(det) < EPSILON)
		{
			auto distance = QVector3D::dotProduct(tri.localFn(), rayOrigin - v0) / tri.localFn().length();
			auto distance1 = QVector3D::dotProduct(tri.localFn(), rayOrigin - v1) / tri.localFn().length();
			auto distance2 = QVector3D::dotProduct(tri.localFn(), rayOrigin - v2) / tri.localFn().length();

			if(std::abs(distance) < EPSILON)
					hit.type = HitType::Degenerate;

			return hit;
		}

		float invDet = 1 / det;

		QVector3D tvec = rayOrigin - v0;
		auto u = QVector3D::dotProduct(tvec, pvec) * invDet;
		if (u < 0 || u > 1)
			return hit;

		// vertex collision
		QVector3D qvec = QVector3D::crossProduct(tvec, v0v1);
		auto v = QVector3D::dotProduct(rayDirection, qvec) * invDet;
		if (v < 0 || u + v > 1)
			return hit;

		auto t = QVector3D::dotProduct(v0v2, qvec) * invDet;
		hit.distance = t;
		hit.intersection = rayOrigin + rayDirection * t;
		if (hit.type != HitType::Miss)
			return hit;
		else if (isBackside)
			hit.type = HitType::BackSide;
		else
			hit.type = HitType::FrontSide;

		return hit;
	}

	void rayIntersect()
	{
		Mesh* testMesh = new Mesh();
		testMesh->addFace(QVector3D(0.0f, 0.0f, 1.0f),
						  QVector3D(1.0f, 0.0f, 0.0f),
						  QVector3D(1.0f, 1.0f, 0.0f));
		testMesh->addFace(QVector3D(1.0f, 1.0f, 0.0f),
						  QVector3D(1.0f, 0.0f, 0.0f),
						  QVector3D(2.0f, 0.0f, 1.0f));

		Mesh* ttMesh = new Mesh();
		ttMesh->addFace(QVector3D(0.0f, 0.0f, 0.0f),
						QVector3D(-4.0f, 0.0f, 0.0f),
						QVector3D(0.0f, 4.0f, 0.0f));

		_rayCaster.reset(new MTRayCaster());
		_rayAccel.reset(new Hix::Engine3D::BVH(*ttMesh, false));
		_rayCaster->addAccelerator(_rayAccel.get());

		QVector3D vertTestOrigin = QVector3D(1.f, 0.f, -4.f);
		QVector3D edgeTestOrigin = QVector3D(0.f, 0.f, -3.f);
		QVector3D planeTestOrigin = QVector3D(0.3f, 0.2f, -0.2f);
		QVector3D parallelTestOrigin = QVector3D(4.f, 2.f, 0.f);
		QVector3D parallelTestOrigin2 = QVector3D(-4.f, -2.f, 0.f);

		QVector3D normalToBottom = QVector3D(0.0f, 0.0f, -1.0f);
		QVector3D normalToRight = QVector3D(-1.0f, 0.0f, 0.0f);
		QVector3D parallelNormal = (QVector3D(-4.0f, 0.0f, 7.0f) - QVector3D(0.0f, 0.0f, 2.0f)).normalized();

		for (auto face = ttMesh->getFaces().cbegin(); face != ttMesh->getFaces().cend(); ++face)
		{
			auto parallelTest = rayIntersectTri(parallelTestOrigin, normalToRight, face);
			auto parallelTest2 = rayIntersectTri(parallelTestOrigin2, normalToRight, face);
			//auto vertTest = rayIntersectTri(vertTestOrigin, normalToRight, face);
			//auto edgeTest = rayIntersectTri(edgeTestOrigin, normalToRight, face);
			//auto planeTest = rayIntersectTri(planeTestOrigin, normalToRight, face);
			qDebug() << "";
		}

	}
	TEST_CASE("BVH Distance Field Accuracy Test", "[BVH DF]")
	{
		SECTION("bvh accuracy test")
		{
			rayIntersect();
		}
	}
}