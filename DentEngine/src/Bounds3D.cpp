#include "Bounds3D.h"

constexpr std::array<float, 6> __iniBound = {
	std::numeric_limits<float>::min()
}

Bounds3D::Bounds3D()
{
}

void Bounds3D::resetBound()
{
}

float Bounds3D::xMax() const
{
	return 0.0f;
}

float Bounds3D::xMin() const
{
	return 0.0f;
}

float Bounds3D::yMax() const
{
	return 0.0f;
}

float Bounds3D::yMin() const
{
	return 0.0f;
}

float Bounds3D::zMax() const
{
	return 0.0f;
}

float Bounds3D::zMin() const
{
	return 0.0f;
}
