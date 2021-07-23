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

		SECTION("bvh accuracy test")
		{
			
			auto bvh = generateDFbyBVHByPQ(*mesh, bound);
			auto all = generateDFbyQueryAllFace(*mesh, bound);


			for (auto i = 0; i < bvh.size(); ++i)
			{
				if (bvh.at(i) != all.at(i))
				{
					qDebug() << i << bvh.at(i) << all.at(i);
				}
			}

			REQUIRE(bvh.size() == all.size());
			for (auto i = 0; i < bvh.size(); ++i)
			{
				if (bvh.at(i) != all.at(i))
				{
					qDebug() << i;
				}
				REQUIRE(bvh.at(i) == all.at(i));
			}

			std::vector<uint8_t> mapped_DF; 
			std::unordered_set<float> pixSet;
			std::unordered_set<uint8_t> pixSet_t;
			auto max_dist = std::max_element(all.begin(), all.end());
			for (auto dist : all)
			{
				auto pixel = (dist / *max_dist) * 255;
				mapped_DF.push_back(pixel);

				if (dist > 0)
					mapped_DF.push_back(255);
				else
					mapped_DF.push_back(0);

				mapped_DF.push_back(0);

			}

			auto lenX = std::ceilf(bound.lengthX() / resolution);
			auto lenY = std::ceilf(bound.lengthY() / resolution);

			stbi_write_png(L"D:/test/ttt.png", lenX, lenY, 3, mapped_DF.data(), lenX*3);
		}
	}
}