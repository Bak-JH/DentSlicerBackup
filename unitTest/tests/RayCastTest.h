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

	void rayIntersect()
	{
		Mesh* testMesh = new Mesh();
		testMesh->addFace(QVector3D(-25.4673, 12.6011, -2.04688),
			QVector3D(-25.5571, 12.6157, -2.14258),
			QVector3D(-25.4106, 12.7183, -2.08105));
		testMesh->addFace(QVector3D(-25.4839f, 12.729f, -2.16016f),
			QVector3D(-25.4106f, 12.7183f, -2.08105f),
			QVector3D(-25.5571f, 12.6157f, -2.14258f));
		testMesh->addFace(QVector3D(-25.4839, 12.729, -2.16016),
			QVector3D(-25.3784, 12.8237, -2.13086),
			QVector3D(-25.4106, 12.7183, -2.08105));

		Mesh* tttt = new Mesh();
		tttt->addFace(QVector3D(0, 0, 0), QVector3D(0, 1, 0), QVector3D(1, 0, 0));

		_rayCaster.reset(new MTRayCaster());
		_rayAccel.reset(new Hix::Engine3D::BVH(*tttt, false));
		_rayCaster->addAccelerator(_rayAccel.get());

		//for (auto x = 0.0f; x <= 1.0f; x += 0.001f)
		//{
		//	for (auto y = 0.0f; y <= 1.0f; y += 0.001f)
		//	{
				auto testOrigin1 = QVector3D(0.3f, 0.3f, 1);
				auto testOrigin2 = QVector3D(0.1f, 0.9f, 1);
				auto testOrigin3 = QVector3D(-1.0f, 0.0f, 1);
				auto testDir = QVector3D(0, 0, -1);
				RayHits tt;
				for (auto x = 1.0f; x > 0.0f; x -= 0.1f)
				{
					for (auto face = tttt->getFaces().cbegin(); face != tttt->getFaces().cend(); ++face)
					{
						
						//tt.push_back(rayIntersectTri(testOrigin, testDir, face));
						auto test1 = rayIntersectTri(testOrigin1, testDir, face);
						auto test2 = rayIntersectTri(testOrigin2, testDir, face);
						auto test3 = rayIntersectTri(testOrigin3, testDir, face);
						//auto parallelTest2 = rayIntersectTri(parallelTestOrigin2, normalToRight, face);
						//auto vertTest = rayIntersectTri(vertTestOrigin, normalToRight, face);
						//auto edgeTest = rayIntersectTri(edgeTestOrigin, normalToRight, face);
						//auto planeTest = rayIntersectTri(planeTestOrigin, normalToRight, face);
						auto stop = 1111;
						
					}
				}
		//	}
		//}

		//QVector3D vertTestOrigin = QVector3D(1.f, 0.f, -4.f);
		//QVector3D edgeTestOrigin = QVector3D(0.f, 0.f, -3.f);
		//QVector3D planeTestOrigin = QVector3D(0.3f, 0.2f, -0.2f);
		//QVector3D parallelTestOrigin = QVector3D(4.f, 2.f, 0.f);
		//QVector3D parallelTestOrigin2 = QVector3D(-4.f, -2.f, 0.f);

		//QVector3D normalToBottom = QVector3D(0.0f, 0.0f, -1.0f);
		//QVector3D normalToRight = QVector3D(-1.0f, 0.0f, 0.0f);
		//QVector3D parallelNormal = (QVector3D(-4.0f, 0.0f, 7.0f) - QVector3D(0.0f, 0.0f, 2.0f)).normalized();

		

	}
	TEST_CASE("BVH Distance Field Accuracy Test", "[BVH DF]")
	{
		SECTION("bvh accuracy test")
		{
			rayIntersect();
		}
	}
}