#pragma once
#include <qvector3d.h>
#include <array>
class Bounds3D
{
public:
	Bounds3D();
	void updateBound(const QVector3D& pt);
	void resetBound();
	float xMax()const;
	float xMin()const;
	float yMax()const;
	float yMin()const;
	float zMax()const;
	float zMin()const;
private:
	std::array<float, 6> _bound;

};

