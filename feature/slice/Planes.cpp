#include "Planes.h"
using namespace Hix::Engine3D;
Hix::Slicer::UniformPlanes::UniformPlanes(float zMin, float zMax, float delta):_delta(delta)
{
	auto plane_z = zMin;
	auto maxWithMargin = zMax + _delta;
	size_t multiple = 0;
	while(plane_z < maxWithMargin)
	{
		plane_z = zMin + _delta * multiple;
		_planes.push_back(plane_z);
		++multiple;
	}
}

size_t Hix::Slicer::UniformPlanes::minIntersectIdx(float z) const
{
	int minIdx = std::ceil((z - _planes[0]) / _delta);
	//float error check
	auto newFloat = _delta * minIdx + _planes[0];
	if (newFloat < z)
		++minIdx;
	minIdx = std::min(std::max(0, minIdx), (int)_planes.size()-1);
	return minIdx;
}

size_t Hix::Slicer::UniformPlanes::maxIntersectIdx(float z) const
{
	int maxIdx = std::floor((z - _planes[0]) / _delta);
	//float error check
	auto newFloat = _delta * maxIdx + _planes[0];
	if (newFloat > z)
		--maxIdx;
	maxIdx = std::min(std::max(0, maxIdx), (int)_planes.size() - 1);
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
			if (z_max <= _planes[0] && z_mid < _planes[0])
				continue;
			if (z_min >= _planes.back() && z_mid > _planes.back())
				continue;
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

