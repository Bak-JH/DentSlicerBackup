#include "Bounds3D.h"
#include "../render/SceneEntity.h"
using namespace Hix::Engine3D;
constexpr std::array<float, 6> __iniBound = {
	std::numeric_limits<float>::lowest(),
	std::numeric_limits<float>::max(),
	std::numeric_limits<float>::lowest(),
	std::numeric_limits<float>::max(),
	std::numeric_limits<float>::lowest(),
	std::numeric_limits<float>::max()
};



Hix::Engine3D::Bounds3D::Bounds3D(QVector3D lengths)
{
	setXLength(lengths.x());
	setYLength(lengths.y());
	setZLength(lengths.z());
}

Bounds3D::Bounds3D():_bound(__iniBound)
{
}

Hix::Engine3D::Bounds3D::Bounds3D(const Hix::Render::SceneEntity& bounded): Bounds3D()
{
	auto vtxCend = bounded.getMesh()->getVertices().cend();
	for (auto vtx = bounded.getMesh()->getVertices().cbegin(); vtx != vtxCend; ++vtx)
	{
		update(vtx.worldPosition());
	}
}

void Hix::Engine3D::Bounds3D::update(const QVector3D& v)
{
	if (v.x() > _bound[0]) _bound[0] = v.x();
	if (v.x() < _bound[1]) _bound[1] = v.x();
	if (v.y() > _bound[2]) _bound[2] = v.y();
	if (v.y() < _bound[3]) _bound[3] = v.y();
	if (v.z() > _bound[4]) _bound[4] = v.z();
	if (v.z() < _bound[5]) _bound[5] = v.z();
}

void Hix::Engine3D::Bounds3D::localBoundUpdate(const Hix::Engine3D::Mesh& mesh)
{
	auto vtxCend = mesh.getVertices().cend();
	for (auto vtx = mesh.getVertices().cbegin(); vtx != vtxCend; ++vtx)
	{
		update(vtx.localPosition());
	}
}

Bounds3D& Hix::Engine3D::Bounds3D::operator+=(const Bounds3D& other)
{
	if (other._bound[0] > _bound[0]) _bound[0] = other._bound[0];
	if (other._bound[1] < _bound[1]) _bound[1] = other._bound[1];
	if (other._bound[2] > _bound[2]) _bound[2] = other._bound[2];
	if (other._bound[3] < _bound[3]) _bound[3] = other._bound[3];
	if (other._bound[4] > _bound[4]) _bound[4] = other._bound[4];
	if (other._bound[5] < _bound[5]) _bound[5] = other._bound[5];
	return *this;
}

void Hix::Engine3D::Bounds3D::translate(const QVector3D& displacement)
{
	_bound[0] += displacement.x();
	_bound[1] += displacement.x();
	_bound[2] += displacement.y();
	_bound[3] += displacement.y();
	_bound[4] += displacement.z();
	_bound[5] += displacement.z();
}

void Hix::Engine3D::Bounds3D::scale(const QVector3D& scales)
{
	_bound[0] *= scales.x();
	_bound[1] *= scales.x();
	_bound[2] *= scales.y();
	_bound[3] *= scales.y();
	_bound[4] *= scales.z();
	_bound[5] *= scales.z();
}

void Bounds3D::reset()
{
	_bound = __iniBound;
}

void Hix::Engine3D::Bounds3D::setXMax(float val)
{
	_bound[0] = val;
}

void Hix::Engine3D::Bounds3D::setXMin(float val)
{
	_bound[1] = val;
}

void Hix::Engine3D::Bounds3D::setYMax(float val)
{
	_bound[2] = val;
}

void Hix::Engine3D::Bounds3D::setYMin(float val)
{
	_bound[3] = val;
}

void Hix::Engine3D::Bounds3D::setZMax(float val)
{
	_bound[4] = val;
}

void Hix::Engine3D::Bounds3D::setZMin(float val)
{
	_bound[5] = val;
}

void Hix::Engine3D::Bounds3D::setXLength(float length)
{
	auto half = length / 2.0f;
	_bound[0] = half;
	_bound[1] = -half;
}

void Hix::Engine3D::Bounds3D::setYLength(float length)
{
	auto half = length / 2.0f;
	_bound[2] = half;
	_bound[3] = -half;
}

void Hix::Engine3D::Bounds3D::setZLength(float length)
{
	auto half = length / 2.0f;
	_bound[4] = half;
	_bound[5] = -half;
}

float Bounds3D::xMax() const
{
	return _bound[0];
}

float Bounds3D::xMin() const
{
	return _bound[1];
}

float Bounds3D::yMax() const
{
	return _bound[2];
}

float Bounds3D::yMin() const
{
	return _bound[3];
}

float Bounds3D::zMax() const
{
	return _bound[4];
}

float Bounds3D::zMin() const
{
	return _bound[5];
}

float Hix::Engine3D::Bounds3D::centreX() const
{
	return (xMax() + xMin())/2.0f;
}

float Hix::Engine3D::Bounds3D::centreY() const
{
	return (yMax() + yMin()) / 2.0f;
}

float Hix::Engine3D::Bounds3D::centreZ() const
{
	return (zMax() + zMin()) / 2.0f;
}

float Hix::Engine3D::Bounds3D::lengthX() const
{
	return std::abs(xMax() - xMin());
}

float Hix::Engine3D::Bounds3D::lengthY() const
{
	return std::abs(yMax() - yMin());
}

float Hix::Engine3D::Bounds3D::lengthZ() const
{
	return std::abs(zMax() - zMin());
}

inline float hyp3D(float a, float b, float c)
{
	return std::sqrt(a * a + b * b + c * c);
}

float Hix::Engine3D::Bounds3D::bbMaxRadius() const
{
	std::array<float, 8> rads{
		hyp3D(_bound[0], _bound[2], _bound[4]),
		hyp3D(_bound[0], _bound[3], _bound[4]),
		hyp3D(_bound[0], _bound[2], _bound[5]),
		hyp3D(_bound[0], _bound[3], _bound[5]),
		hyp3D(_bound[1], _bound[2], _bound[4]),
		hyp3D(_bound[1], _bound[3], _bound[4]),
		hyp3D(_bound[1], _bound[2], _bound[5]),
		hyp3D(_bound[1], _bound[3], _bound[5]),
	};
	return *std::max_element(rads.cbegin(), rads.cend());
}

QVector3D Hix::Engine3D::Bounds3D::centre() const
{
	return QVector3D(centreX(), centreY(), centreZ());
}

QVector3D Hix::Engine3D::Bounds3D::lengths() const
{
	return QVector3D(lengthX(), lengthY(), lengthZ());
}

const std::array<float, 6>& Hix::Engine3D::Bounds3D::bound() const
{
	return _bound;
}

QVector3D Hix::Engine3D::Bounds3D::displaceWithin(const Bounds3D& child, QVector3D displacement) const
{
	QVector3D result;
	auto maxDisplacemnts = calculateMaxDisplacement(child);
	for (size_t i = 0; i < 3; ++i)
	{
		if (displacement[i] > 0)
			result[i] = std::min(displacement[i], maxDisplacemnts[i * 2 + 1]);
		else
			result[i] = std::max(displacement[i], maxDisplacemnts[i * 2]);
	}
	return result;
}

std::array<float, 6> Hix::Engine3D::Bounds3D::calculateMaxDisplacement(const Bounds3D & child) const
{
	//min, max, min, max....
	std::array<float, 6> result{
		_bound[1] - child._bound[1],
		_bound[0] - child._bound[0],
		_bound[3] - child._bound[3],
		_bound[2] - child._bound[2],
		_bound[5] - child._bound[5],
		_bound[4] - child._bound[4]
	};
	return result;
	
}

bool Hix::Engine3D::Bounds3D::contains(const Hix::Engine3D::Bounds3D& other)const
{
	if (other.xMin() >= xMin() &&
		other.xMax() <= xMax() &&
		other.yMin() >= yMin() &&
		other.yMax() <= yMax() &&
		other.zMin() >= zMin() &&
		other.zMax() <= zMax())
	{
		return true;
	}
	return false;
}



bool Hix::Engine3D::Bounds3D::intersects(const Hix::Engine3D::Bounds3D& other)const
{
	if (other.xMin() > xMax() ||
		other.xMax() < xMin() ||
		other.yMin() > yMax() ||
		other.yMax() < yMin() ||
		other.zMin() > zMax() ||
		other.zMax() < zMin())
	{
		return false;
	}
	return true;
}

bool Hix::Engine3D::Bounds3D::intersects2D(const Hix::Engine3D::Bounds3D& other) const
{
	if (other.xMin() > xMax() ||
		other.xMax() < xMin() ||
		other.yMin() > yMax() ||
		other.yMax() < yMin())
	{
		return false;
	}
	return true;
}

