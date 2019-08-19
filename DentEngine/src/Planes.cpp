#include "Planes.h"

using namespace Hix::Engine3D;
Hix::Slicer::UniformPlanes::UniformPlanes(const Hix::Engine3D::Mesh* mesh, float delta): _mesh(mesh), _delta(delta)
{
	size_t idx_max = ceil((_mesh->z_max() - _mesh->z_min()) / delta);
	_planes.reserve(idx_max);
	for (int i = 0; i < idx_max; ++i)
	{
		float plane_z = _mesh->z_min() + delta * i;
		_planes.push_back(plane_z);
		qDebug() << "build Uniform Planes at height z " << plane_z;
	}
}

size_t Hix::Slicer::UniformPlanes::minIntersectIdx(float z) const
{
	int minIdx = (int)((z - _planes[0]) / _delta) + 1;
	minIdx = std::max(0, minIdx);
	return minIdx;
}

size_t Hix::Slicer::UniformPlanes::maxIntersectIdx(float z) const
{
	int maxIdx = (int)((z - _planes[0]) / _delta);
	maxIdx = std::min((int)_planes.size() - 1, maxIdx);
	return maxIdx;
}

const std::vector<float>& Hix::Slicer::UniformPlanes::getPlanesVector() const
{
	return _planes;
}

void Hix::Slicer::UniformPlanes::buildTriangleLists()
{
	size_t faceCnt = _mesh->getFaces().size();
	// Create List of list
	_triangleList = std::vector<std::unordered_set<FaceConstItr>>(_planes.size());

	// Uniform Slicing O(n)
	if (_delta > 0) {
		auto& faces = _mesh->getFaces();
		for (auto mf = faces.cbegin(); mf != faces.cend(); ++mf)
		{
			auto sortedZ = mf->sortZ();
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
				_triangleList[i].insert(mf);
			}
		}
	}
}

const std::vector<std::unordered_set<Hix::Engine3D::FaceConstItr>>& Hix::Slicer::UniformPlanes::getTrigList() const
{
	return _triangleList;
}

