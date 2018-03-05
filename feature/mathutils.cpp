#include "mathutils.h"
#include <cfloat>

bool RayIntersectsTriangle(QVector3D rayOrigin,
                           QVector3D rayDirection,
                           QVector3D vertex0, QVector3D vertex1, QVector3D vertex2, // CCW order
                           QVector3D& outIntersectionPoint)
{
    QVector3D edge1, edge2, h, s, q;
    float a, f, u, v;
    edge1 = vertex1 - vertex0;
    edge2 = vertex2 - vertex0;

    h = QVector3D::crossProduct(rayDirection, edge2);
    a = QVector3D::dotProduct(edge1, h);

    if (a > -FLT_EPSILON && a < FLT_EPSILON)
        return false;

    f = 1/a;
    s = rayOrigin - vertex0;
    u = f * QVector3D::dotProduct(s, h);
    if (u < 0.0 || u > 1.0)
        return false;

    q = QVector3D::crossProduct(s, edge1);
    v = f * QVector3D::dotProduct(rayDirection, q);
    if (v < 0.0 || u + v > 1.0)
        return false;

    float t = f * QVector3D::dotProduct(edge2, q);
    if (t > FLT_EPSILON) {
        outIntersectionPoint = rayOrigin + rayDirection * t;
        return true;
    } else {
        return false;
    }
}
