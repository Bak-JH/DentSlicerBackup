#pragma once
#include "BVHTest.h"

namespace BVHTest
{
	using namespace Hix;
	using namespace Hix::Engine3D;

	TEST_CASE("BVH Distance Field BenchMarking Test", "[BVH DF BENCH]")
	{
		Bounds3D bound;
		const std::filesystem::path& path = L"D:/test/tttttt.stl";

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


		SECTION("compare with querying all faces")
		{


			//BENCHMARK("Query All Faces")
			//{
			//	generateDFbyBVH(*mesh, bound);
			//};
			BENCHMARK("Query BVH Faces By Priority Queue")
			{
				generateDFbyBVHByPQ(*mesh, bound);
			};

		}
	}
}