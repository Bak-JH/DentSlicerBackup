// Float3DVectorHash.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
struct QVector3D
{
	float x;
	float y;
	float z;


};

namespace  SlicingConfiguration
{
	constexpr float max_buildsize_x = 1000;//1000000/resolution;
	constexpr float vertex_inbound_distance = 0.002;//0.03;//(float)1/resolution; // resolution in mm (0.0001 and 0.0009 are same, 1 micron)
	float vertexDistance(QVector3D a, QVector3D b) {
		QVector3D dv = { a.x - b.x, a.y - b.y, a.z - b.z };
		float distance = dv.x * dv.x + dv.y * dv.y + dv.z * dv.z;
		return distance;
	}
	bool isVectorsEqual(QVector3D a, QVector3D b)
	{
		return vertexDistance(a, b) <= SlicingConfiguration::vertex_inbound_distance * SlicingConfiguration::vertex_inbound_distance;
	}
}



class hasher
{
public:
	virtual uint32_t hash(QVector3D v) = 0;
};

class diridiriHash: public hasher
{
public:
	uint32_t hash(QVector3D v)override // max build size = 1000mm, resolution = 1 micron
	{
		return	(uint32_t(((v.x + SlicingConfiguration::vertex_inbound_distance / 2) / SlicingConfiguration::vertex_inbound_distance)) ^ \
				(uint32_t(((v.y + SlicingConfiguration::vertex_inbound_distance / 2) / SlicingConfiguration::vertex_inbound_distance)) << 10) ^ \
				(uint32_t(((v.z + SlicingConfiguration::vertex_inbound_distance / 2) / SlicingConfiguration::vertex_inbound_distance)) << 20));
	}
};




TEMPLATE_TEST_CASE("QVector3D hasher test", "hash", diridiriHash)
{
	TestType hasher;
	//hasher* hasher = dynamic_cast<hasher*>(&t);

	SECTION("same float")
	{
		QVector3D a{ 0.0001, -0.0001, 20000 };
		QVector3D b{ 0.0001, -0.0001, 20000 };

		auto aHash = hasher.hash(a);
		auto bHash = hasher.hash(b);
		REQUIRE(aHash == bHash);

	}
	SECTION("very different")
	{
		QVector3D a{ 0.0001, -0.0001, 20000 };
		QVector3D b{ 12123123123.0001, -222222220.0001, 0 };

		auto aHash = hasher.hash(a);
		auto bHash = hasher.hash(b);
		REQUIRE(aHash != bHash);

	}
	SECTION("z offset by slicer option")
	{
		QVector3D a{ 0.0001, -0.0001, 20000 };
		QVector3D b{ 0.0001, -0.0001, 20000 + SlicingConfiguration::vertex_inbound_distance};

		auto aHash = hasher.hash(a);
		auto bHash = hasher.hash(b);
		REQUIRE(aHash == bHash);
	}
	SECTION("z offset by slicer option * 1.2s")
	{
		QVector3D a{ 0.0001, -0.0001, 20000 };
		QVector3D b{ 0.0001, -0.0001, 20000 + SlicingConfiguration::vertex_inbound_distance*1.2 };

		auto aHash = hasher.hash(a);
		auto bHash = hasher.hash(b);
		REQUIRE(aHash == bHash);

	}
	SECTION("z offset by slicer option * 2")
	{
		QVector3D a{ 0.0001, -0.0001, 20000 };
		QVector3D b{ 0.0001, -0.0001, 20000 + SlicingConfiguration::vertex_inbound_distance * 2 };

		auto aHash = hasher.hash(a);
		auto bHash = hasher.hash(b);
		REQUIRE(aHash != bHash);

	}
	SECTION("smaller model")
	{
		QVector3D a{ 0.0001 + +SlicingConfiguration::vertex_inbound_distance, -0.0001, 0.001 };
		QVector3D b{ 0.0001, -0.0001, 0.001 };

		auto aHash = hasher.hash(a);
		auto bHash = hasher.hash(b);
		REQUIRE(aHash != bHash);

	}
}