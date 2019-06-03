#pragma once

#include <QVector3D>
#include <QMatrix4x4>
#include <QQuaternion>
namespace Utils
{
    namespace Math
    {
        // Reference: https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
        bool RayIntersectsTriangle(QVector3D rayOrigin,
                                   QVector3D rayDirection,
                                   QVector3D vertex0, QVector3D vertex1, QVector3D vertex2, // CCW order
                                   QVector3D& outIntersectionPoint);

        QMatrix4x4 quatToMat(QQuaternion q);

        float round(float num, int precision);

        bool doubleAreSame(double a, double b);

		inline float intPointDistance(QPoint a, QPoint b)
		{
			return std::sqrt(std::pow(a.x() - b.x(), 2) + std::pow(a.y() - b.y(), 2));
		}
		//doesn't work on inf
		inline bool sameSign(float a, float b) {
			return a * b >= 0.0f;
		}

    }

}

