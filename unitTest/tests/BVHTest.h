#pragma once
#pragma once
#include <iostream>
#include <string>
#include <queue>
#include <functional>
#include <unordered_set>
#include <chrono>  // for high_resolution_clock


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

	float resolution = 1.0f;

	std::vector<float> generateDFbyBVH(const Mesh& mesh, Bounds3D& bound)
	{
		std::vector<float> DF;
		std::unique_ptr<Hix::Engine3D::RayAccelerator> bvhAccel;
		bvhAccel.reset(new Hix::Engine3D::BVH(mesh, false));

		int xMin = bound.xMin();
		int xMax = bound.xMax();
		int yMin = bound.yMin();
		int yMax = bound.yMax();
		int zMin = bound.zMin();
		int zMax = bound.zMax();

		int cnt = 0;

		for (float z = yMin; z < zMax; z += resolution)
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

	std::vector<float> generateDFbyQueryAllFace(const Mesh& mesh, Bounds3D& bound)
	{
		std::vector<float> DF;

		int xMin = bound.xMin();
		int xMax = bound.xMax();
		int yMin = bound.yMin();
		int yMax = bound.yMax();
		int zMin = bound.zMin();
		int zMax = bound.zMax();

		int cnt = 0;
		for (float z = yMin; z < zMax; z += resolution)
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
					DF.push_back(closestDistance);
				}
			}
		}

		return DF;
	}
}