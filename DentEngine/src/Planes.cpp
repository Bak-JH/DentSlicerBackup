#include "Planes.h"

using namespace Hix::Engine3D;
Hix::Slicer::UniformPlanes::UniformPlanes(float zMin, float zMax, float delta):_delta(delta)
{
	size_t idx_max = ceil((zMax - zMin) / delta);
	_planes.reserve(idx_max);
	for (int i = 0; i < idx_max; ++i)
	{
		float plane_z = zMin + delta * i;
		_planes.push_back(plane_z);
	}
}

size_t Hix::Slicer::UniformPlanes::minIntersectIdx(float z) const
{
	int minIdx = (int)((z - _planes[0]) / _delta) + 1;
	minIdx = std::max(0, minIdx);

	//float error check
	if (minIdx < _planes.size() && _planes[minIdx] < z)
		++minIdx;

	return minIdx;
}

size_t Hix::Slicer::UniformPlanes::maxIntersectIdx(float z) const
{
	int maxIdx = (int)((z - _planes[0]) / _delta);
	maxIdx = std::min((int)_planes.size() - 1, maxIdx);
	//float error check
	if (maxIdx >= 0 && _planes[maxIdx] > z)
		--maxIdx;
	return maxIdx;
}

const std::vector<float>& Hix::Slicer::UniformPlanes::getPlanesVector() const
{
	return _planes;
}

std::vector<std::unordered_set<Hix::Engine3D::FaceConstItr>> Hix::Slicer::UniformPlanes::buildTriangleLists(const Hix::Engine3D::Mesh* mesh)const
{
	size_t faceCnt = mesh->getFaces().size();
	// Create List of list
	std::vector<std::unordered_set<FaceConstItr>> result(_planes.size());
	// Uniform Slicing O(n)
	if (_delta > 0) {
		auto& faces = mesh->getFaces();
		for (auto mf = faces.cbegin(); mf != faces.cend(); ++mf)
		{
			auto sortedZ = mf.sortZ();
			float z_min = sortedZ[0];
			float z_mid = sortedZ[1];
			float z_max = sortedZ[2];
			size_t minIdx = minIntersectIdx(z_min);
			size_t maxIdx = maxIntersectIdx(z_max);
			if (minIdx > maxIdx)
				continue;
			if (z_min == z_max)
				continue;
			//just the tip~
			if (_planes[minIdx] == z_min && z_min != z_mid)
			{
				++minIdx;
			}
			else if (_planes[maxIdx] == z_max && z_max != z_mid)
			{
				--maxIdx;
			}
			for (int i = minIdx; i <= maxIdx; ++i)
			{
				result[i].insert(mf);
			}
		}
	}
	return result;
}

