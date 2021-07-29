// TrackedIndexedList.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "BVHTest.h"

#define STBI_WINDOWS_UTF16
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "feature/slice/include/stb_image_write.h"

//#include "../IteratorWrapper.h"

namespace BVHTest
{
	using namespace Hix;
	using namespace Hix::Engine3D;

	TEST_CASE("BVH Distance Field Accuracy Test", "[BVH DF]")
	{
		Bounds3D bound;
		const std::filesystem::path& path = L"D:/test/dddddd.stl";

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
		bound = bound.centred();

		int xMin = std::floorf(bound.xMin());
		int xMax = std::ceilf(bound.xMax());
		int yMin = std::floorf(bound.yMin());
		int yMax = std::ceilf(bound.yMax());
		int zMin = std::floorf(bound.zMin());
		int zMax = std::ceilf(bound.zMax());

		int lengthX = std::ceilf((xMax - xMin) / _resolution);
		int lengthY = std::ceilf((yMax - yMin) / _resolution);
		int lengthZ = std::ceilf((zMax - zMin) / _resolution);

		SECTION("bvh accuracy test")
		{
			
			auto bvh = generateDFbyBVHByPQ(*mesh, bound);
			//auto all = generateDFbyQueryAllFace(*mesh, bound);

			//REQUIRE(bvh.size() == all.size());
			//for (auto i = 0; i < bvh.size(); ++i)
			//{
			//	if (bvh.at(i) != all.at(i))
			//	{
			//		qDebug() << i;
			//	}
			//	REQUIRE(bvh.at(i) == all.at(i));
			//}

			std::vector<uint8_t> mapped_DF;
			auto max_dist = std::max_element(bvh.begin(), bvh.end());

			for (auto dist : bvh)
			{
					auto pixel = (dist / *max_dist) * 255;
					mapped_DF.push_back(pixel);
			}

			auto lenX = std::ceilf(bound.lengthX() / _resolution);
			auto lenY = std::ceilf(bound.lengthY() / _resolution);

			qDebug() << lenX << lenY;

			stbi_write_png(L"D:/test/ttt.png", lenX, lenY, 1, mapped_DF.data(), lenX);

			//std::vector<uint8_t> mapped_DF_a;
			//max_dist = std::max_element(all.begin(), all.end());
			//for (auto dist : all)
			//{
			//	auto pixel = (dist / *max_dist) * 255;
			//	mapped_DF_a.push_back(pixel);

			//}

			//stbi_write_png(L"D:/test/ttt5555.png", lenX, lenY, 1, mapped_DF_a.data(), lenX);
		}
	}
}