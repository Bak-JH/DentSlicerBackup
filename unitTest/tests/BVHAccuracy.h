// TrackedIndexedList.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "BVHTest.h"
//
//#define STBI_WINDOWS_UTF16
//#define STB_IMAGE_WRITE_IMPLEMENTATION
//#include "feature/slice/include/stb_image_write.h"

//#include "../IteratorWrapper.h"

namespace BVHTest
{
	using namespace Hix;
	using namespace Hix::Engine3D;

	TEST_CASE("BVH Distance Field Accuracy Test", "[BVH DF]")
	{
		Bounds3D bound;
		const std::filesystem::path& path = L"D:/test/mmmmmm.stl";

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
			int index = 0;
			for (auto z = _zMin; z <= _zMax; z += _resolution)
			{
				//auto bvh = generateDFbyRayNormal(*mesh, bound, z);
				auto allDF = generateDFbyQueryAllFace(*mesh, bound, z);
				auto allSDF = generateSDFbyQueryAllFace(*mesh, bound, z);

				qDebug() << "tttt";
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
				auto max_dist = std::max_element(allDF.begin(), allDF.end());

				for (auto dist : allSDF)
				{
					//auto pixel = (dist / *max_dist) * 255;
					//mapped_DF.push_back(pixel);
					if (dist < 0.0f)
					{
						mapped_DF.push_back(0);
					}
					else
					{
						mapped_DF.push_back(255);
					}
				}
				std::stringstream idxStream;
				idxStream << index;
				auto idxStr = idxStream.str() + ".png";
				std::filesystem::path file = std::filesystem::path("D:/test/ttt") / (idxStr);
				stbi_write_png(file.c_str(), _lengthX, _lengthY, 1, mapped_DF.data(), _lengthX);
				++index;
			}

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