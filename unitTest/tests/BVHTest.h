#pragma once
#pragma once
#include <iostream>
#include <string>
#include <queue>
#include <functional>
#include <unordered_set>
#include <chrono>  // for high_resolution_clock
#include <execution>
#include <iterator>

#include <boost/algorithm/string.hpp>

#include "../catch.hpp"
#include "../../common/IteratorWrapper.h"

#include "feature/sampling.h"

#include "Mesh/RayAccelerator.h"
#include "Mesh/BVH.h"
#include "Mesh/HixBVH.h"
#include "../../glmodel.h"
//#include "../../common/Tree/BVHBuilder.h"
#include "application/SceneManager.h"
#include "application/ApplicationManager.h"
#include "render/Bounds3D.h"


namespace BVHTest
{
	using namespace Hix;
	using namespace Hix::Engine3D;

	float resolution = 2.0f;

	std::vector<float> generateDFbyBVH(const Mesh& mesh, Bounds3D& bound)
	{
		std::vector<float> DF;
		std::unique_ptr<Hix::Engine3D::RayAccelerator> bvhAccel;
		bvhAccel.reset(new Hix::Engine3D::BVH(mesh, false));

		int xMin = std::floorf(bound.xMin());
		int xMax = std::ceilf(bound.xMax());
		int yMin = std::floorf(bound.yMin());
		int yMax = std::ceilf(bound.yMax());
		int zMin = std::floorf(bound.zMin());
		int zMax = std::ceilf(bound.zMax());

		int cnt = 0;

		for (float z = zMin; z < zMax; z += resolution)
		{
			for (float y = yMin; y < yMax; y += resolution)
			{
				for (float x = xMin; x < xMax; x += resolution)
				{
					QVector3D currPt = QVector3D(x, y, z);
					auto bvhdist = bvhAccel->getClosestDistance(currPt);


					DF.push_back(bvhdist);
				}
			}
		}

		return DF;
	}

	std::vector<float> generateDFbyBVHByPQ(const Mesh& mesh, Bounds3D& bound)
	{
		std::unique_ptr<Hix::Engine3D::RayAccelerator> bvhAccel;
		bvhAccel.reset(new Hix::Engine3D::BVH(mesh, false));

		qDebug() << bound.centre();

		int xMin = std::floorf(bound.xMin());
		int xMax = std::ceilf(bound.xMax());
		int yMin = std::floorf(bound.yMin());
		int yMax = std::ceilf(bound.yMax());
		int zMin = std::floorf(bound.zMin());
		int zMax = std::ceilf(bound.zMax());

		int lengthX = std::ceilf((xMax - xMin)/resolution);
		int lengthY = std::ceilf((yMax - yMin)/resolution);
		int lengthZ = std::ceilf((zMax - zMin)/resolution);

		std::vector<float> DF(lengthX*lengthY*lengthZ);

		int cnt = 0;


		std::vector<float> zVec;
		for (float z = zMin; z < zMax; z += resolution)
			zVec.push_back(z);

		std::for_each(std::execution::par_unseq, std::begin(zVec), std::end(zVec), [&](int z)
			{
				for (float y = yMin; y < yMax; y += resolution)
				{
					for (float x = xMin; x < xMax; x += resolution)
					{
						QVector3D currPt = QVector3D(x, y, z);
						auto bvhdist = bvhAccel->getClosestDistance(currPt);
						//qDebug() << bvhdist;


						DF[ ((x + std::abs(xMin))/resolution) +
							(((y + std::abs(yMin))/resolution) * lengthX) +
							((z + std::abs(zMin))/resolution) * (lengthX * lengthY)] = bvhdist;
					}
				}
			});

		return DF;
	}


	std::vector<float> generateDFbyQueryAllFace(const Mesh& mesh, Bounds3D& bound)
	{
		std::vector<float> DF;

		int test = 10;

		int xMin = std::floorf(bound.xMin());
		int xMax = std::ceilf(bound.xMax());
		int yMin = std::floorf(bound.yMin());
		int yMax = std::ceilf(bound.yMax());
		int zMin = std::floorf(bound.zMin());
		int zMax = std::ceilf(bound.zMax());

		int cnt = 0;
		for (float z = zMin; z < zMax; z += resolution)
		{
			for (float y = yMin; y < yMax; y += resolution)
			{
				for (float x = xMin; x < xMax; x += resolution)
				{
					auto closestDistance = std::numeric_limits<float>::max();
					QVector3D closestPoint;
					FaceConstItr closestFace;
					QVector3D currPt = QVector3D(x, y, z);

					for (auto face = mesh.getFaces().begin(); face != mesh.getFaces().end(); ++face)
					{
						auto tmpClosest = getClosestVertex(currPt, face);
						auto tempDistance = (currPt - tmpClosest).lengthSquared();
						if (tempDistance < closestDistance)
						{
							closestDistance = tempDistance;
							closestPoint = tmpClosest;
							closestFace = face;
						}
					}

					auto normalST = currPt - closestPoint;
					normalST.normalize();
					auto dot = QVector3D::dotProduct(normalST, closestFace.localFn());
					auto closestDistance_sign = dot > 0.0f ? 1.0f : -1.0f;

					closestDistance *= closestDistance_sign;

					DF.push_back(closestDistance);
				}
			}
		}

		return DF;
	}
}