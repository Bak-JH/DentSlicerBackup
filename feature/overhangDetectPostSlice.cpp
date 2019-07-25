#include "overhangDetectPostSlice.h"
#include "../DentEngine/src/mesh.h"
#include "../slice/slicingengine.h"


struct Volume
{
	std::vector<const Slice*> slices; //slice that makes up this volume
};
struct OverhangWithSlice
{
	
};
void detectOverhangWithSlices(const Slices& slices)
{
	if (slices.size() > 1)
	{
		auto upperSlice = slices.cbegin() + 1;
		auto lowerSlice = slices.cbegin();
		while (upperSlice != slices.end())
		{

		}
	}
}
std::vector<QVector3D> Hix::SupportGeneration::PostSlice::OverhangDetectPostSlice(const Engine3D::Mesh& shellMesh)
{
	std::vector<QVector3D> overhangs;
	Slices shellSlices;
	Slicer::slice(&shellMesh, &shellSlices);


	return overhangs;
}
