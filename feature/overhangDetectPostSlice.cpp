#include "overhangDetectPostSlice.h"
#include "../DentEngine/src/mesh.h"
#include "../slice/slicingengine.h"
std::vector<QVector3D> Hix::SupportGeneration::PostSlice::OverhangDetectPostSlice(const Engine3D::Mesh& shellMesh)
{
	std::vector<QVector3D> overhangs;
	Slices shellSlices;
	Slicer::slice(&shellMesh, &shellSlices);


	return overhangs;
}
