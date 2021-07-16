// TrackedIndexedList.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "BVHTest.h"


//#include "../IteratorWrapper.h"

namespace BVHTest
{
	using namespace Hix;
	using namespace Hix::Engine3D;

	TEST_CASE("BVH Distance Field Accuracy Test", "[BVH DF]")
	{
		Bounds3D bound;
		const std::filesystem::path& path = L"D:/test/20mm_cube.stl";

		auto mesh = new Hix::Engine3D::Mesh();
		std::fstream file(path);
		if (!file.is_open())
			return;

		if (boost::iequals(path.extension().string(), ".stl")) {

			if (!FileLoader::loadMeshSTL(mesh, file))
			{
				std::fstream fileBinary(path, std::ios_base::in | std::ios_base::binary);
				FileLoader::loadMeshSTL_binary(mesh, fileBinary);
			}
		}

		const Mesh* constMesh = new Mesh(*mesh);

		bound.localBoundUpdate(*mesh);
		bound.centred();

		int xMin = bound.xMin();
		int xMax = bound.xMax();
		int yMin = bound.yMin();
		int yMax = bound.yMax();
		int zMin = bound.zMin();
		int zMax = bound.zMax();

		SECTION("bvh accuracy test")
		{

			auto bvh = generateDFbyQueryAllFace(*mesh, bound);
			auto all = generateDFbyBVH(*mesh, bound);

			REQUIRE(bvh.size() == all.size());
			for (auto i = 0; i < bvh.size(); ++i)
			{
				REQUIRE(bvh.at(i) == all.at(i));
			}


			//auto bvhDF = generateDFbyBVH(*mesh, bound);
			//auto allDF = generateDFbyQueryAllFace(*mesh, bound);

			//for (auto idx = 0; idx < allDF.size()-1; ++idx)
			//{
			//	if (bvhDF.at(idx) != allDF.at(idx))
			//	{
			//		qDebug() << bvhDF.at(idx);
			//		qDebug() << allDF.at(idx);
			//		qDebug() << "dddd";
			//	}
			//	REQUIRE(bvhDF.at(idx) == allDF.at(idx));

			//}

			//std::vector<uint8_t> mapped_DF; 
			//std::unordered_set<float> pixSet;
			//std::unordered_set<uint8_t> pixSet_t;
			//auto max_dist = std::max_element(DF.begin(), DF.end());
			//for (auto dist : DF)
			//{
			//	auto pixel = (dist / *max_dist) * 255;
			//	mapped_DF.push_back(pixel);
			//	++cnt;

			//}

			//uint8_t res_x = (xMax - xMin) / resolution;
			//uint8_t res_y = (yMax - yMin) / resolution;

			//stbi_write_png(L"D:/test/ttt.png", res_x, res_y, 1, mapped_DF.data(), res_x);
		}
	}
}