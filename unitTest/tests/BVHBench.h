#pragma once
#include "BVHTest.h"

namespace BVHTest
{
	using namespace Hix;
	using namespace Hix::Engine3D;

	TEST_CASE("BVH Distance Field BenchMarking Test", "[BVH DF BENCH]")
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


		SECTION("compare with querying all faces")
		{


			BENCHMARK("Query All Faces")
			{
				generateDFbyQueryAllFace(*mesh, bound);
			};
			BENCHMARK("Query BVH Faces")
			{
				generateDFbyBVH(*mesh, bound);
			};
		}
	}
}