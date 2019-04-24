#ifndef MATHUTILS_H
#define MATHUTILS_H

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
    }

}

#endif // MATHUTILS_H
