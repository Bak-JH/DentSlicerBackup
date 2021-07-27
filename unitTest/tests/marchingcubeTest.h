#pragma once
#include "../catch.hpp"
#include <pcl/surface/marching_cubes_hoppe.h>

namespace BVHTest
{

	TEST_CASE("Marching Cubes Accuracy Test", "[MarchingCube]")
	{
		SECTION("marching cube accuracy test")
		{
			pcl::MarchingCubesHoppe<pcl::PointNormal> mc;
			pcl::PolygonMesh::Ptr triangles(new pcl::PolygonMesh);
		}
	}
}