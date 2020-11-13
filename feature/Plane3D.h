#pragma once
#include <QVector3D>
#include <vector>
#include <algorithm>
#include <numeric>

namespace Hix
{
	namespace Plane3D
	{
		struct PDPlane
		{
			QVector3D point;
			QVector3D normal;
		};

		template <typename PointsCont>
		PDPlane bestFittingPlane(const PointsCont& points)
		{
			PDPlane plane;
			if (points.size() < 3) {
				return plane; // At least three points required
			}

			QVector3D sum = std::reduce(points.begin(), points.end());
			auto centroid = sum * (1.0 / points.size());
			std::vector<QVector3D> rs;
			std::vector<std::array<float, 6>> covMats;
			rs.reserve(points.size());
			covMats.reserve(points.size());
			// Calc full 3x3 covariance matrix, excluding symmetries:
			std::transform(points.begin(), points.end(), std::back_inserter(rs), [&centroid](const QVector3D& e) { return e - centroid; });
			std::transform(rs.begin(), rs.end(), std::back_inserter(covMats), [&centroid](const QVector3D& r) -> std::array<float,6> {

				return {
					r.x() * r.x(),
					r.x() * r.y(),
					r.x() * r.z(),
					r.y() * r.y(),
					r.y() * r.z(),
					r.z() * r.z() };
				});
			auto covaSum = std::reduce(covMats.begin(), covMats.end(), std::array<float, 6>{}, [](const std::array<float, 6>& a, std::array<float, 6>& b) {
				std::array<float, 6> result;
				std::transform(a.begin(), a.end(), b.begin(), result.begin(), std::plus<float>());
				return result;
				});
			float xx = covaSum[0], xy = covaSum[1], xz = covaSum[2], yy = covaSum[3], yz = covaSum[4], zz = covaSum[5];
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

	}
}

