#include "Plane3D.h"

Hix::Plane3D::PDPlane Hix::Plane3D::bestFittingPlane(const std::vector<QVector3D>& points)
{
	PDPlane plane;
	if (points.size() < 3) {
		return plane; // At least three points required
	}

	QVector3D sum;
	for (auto& p : points)
	{
		sum += p;
	}

	auto centroid = sum * (1.0 / points.size());
	float xx = 0, xy = 0, xz = 0, yy = 0, yz = 0, zz = 0;
	// Calc full 3x3 covariance matrix, excluding symmetries:
	for (auto& p : points)
	{
		auto r = p - centroid;
		xx += r.x() * r.x();
		xy += r.x() * r.y();
		xz += r.x() * r.z();
		yy += r.y() * r.y();
		yz += r.y() * r.z();
		zz += r.z() * r.z();
	}
	auto det_x = yy * zz - yz * yz;
	auto det_y = xx * zz - xz * xz;
	auto det_z = xx * yy - xy * xy;

	auto det_max = std::max({ det_x, det_y, det_z });
	if (det_max <= 0.0) {
		return plane; // The points don't span a plane
	}

	// Pick path with best conditioning:
	QVector3D dir;
	if (det_max == det_x) {
		dir = QVector3D(
			det_x,
			xz * yz - xy * zz,
			xy * yz - xz * yy
		);
	}
	else if (det_max == det_y) {
		dir = QVector3D(
			xz * yz - xy * zz,
			det_y,
			xy * xz - yz * xx
		);
	}
	else {
		dir = QVector3D(
			xy * yz - xz * yy,
			xy * xz - yz * xx,
			det_z
		);
	};
	plane.normal = dir.normalized();
	plane.point = centroid;
	return plane;
}