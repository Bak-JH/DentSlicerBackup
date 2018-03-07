#include <Qt3DRender/qbuffer.h>
#include <Qt3DRender/qbufferdatagenerator.h>
#include <Qt3DRender/qattribute.h>
#include <qmath.h>
#include <QVector3D>
#include <QTextLayout>
#include <QTime>
#include <cfloat>
#include <cmath>

#include "qtriangulator_p.h"
#include "text3dgeometrygenerator.h"
#include "mathutils.h"

using IndexType = unsigned int;

struct TriangulationData {
    struct Outline {
        int begin;
        int end;
    };

    QVector<QVector3D> vertices;
    QVector<IndexType> indices;
    QVector<Outline> outlines;
    QVector<IndexType> outlineIndices;
    bool inverted;
};

TriangulationData triangulate(const QString &text, const QFont &font)
{
    TriangulationData result;
    int beginOutline = 0;

    QPainterPath path;
    path.setFillRule(Qt::WindingFill);
    path.addText(0, 0, font, text);
    QList<QPolygonF> polygons = path.toSubpathPolygons(QTransform().scale(1.0f, -1.0f));

    if (polygons.size() == 0)
        return result;

    const int prevNumIndices = result.indices.size();

    path = QPainterPath();
    path.setFillRule(Qt::WindingFill);
    for (QPolygonF& p : polygons)
        path.addPolygon(p);

    QPolylineSet polylines = qPolyline(path, QTransform(), 1, true);
    QVector<IndexType> tmpIndices;
    tmpIndices.resize(polylines.indices.size());
    memcpy(tmpIndices.data(), polylines.indices.data(), polylines.indices.size() * sizeof(IndexType));

    int lastIndex = 0;
    for (const IndexType idx : tmpIndices) {
       if (idx == std::numeric_limits<IndexType>::max()) {
           const int endOutline = lastIndex;
           result.outlines.push_back({beginOutline, endOutline});
           beginOutline = endOutline;
       } else {
           result.outlineIndices.push_back(idx);
           ++lastIndex;
       }
    }

    const QTriangleSet triangles = qTriangulate(path, QTransform(), 1, true);
    result.indices.resize(result.indices.size() + triangles.indices.size());
    memcpy(&result.indices[prevNumIndices], triangles.indices.data(), triangles.indices.size() * sizeof(IndexType));
    for (int i = prevNumIndices, m = result.indices.size(); i < m; ++i)
        result.indices[i] += result.vertices.size();

    result.vertices.reserve(triangles.vertices.size() / 2);
    for (int i = 0, m = triangles.vertices.size(); i < m; i += 2)
           result.vertices.push_back(QVector3D(triangles.vertices[i] / font.pointSizeF(), triangles.vertices[i + 1] / font.pointSizeF(), 0.0f));

    return result;
}

inline QVector3D mix(const QVector3D &a, const QVector3D &b, float ratio)
{
    return a + (b - a) * ratio;
}

void generateText3DGeometry(QVector3D** vertices, int* verticesSize,
                            unsigned int** indices, int* indicesSize,
                            QFont font, QString text, float depth,
                            const QVector3D* originalVertices,
                            const int originalVerticesCount,
                            const QVector3D normalVector,
                            const QMatrix4x4& transform,
                            const QMatrix4x4& normalTransform)
{
    float edgeSplitAngle = 90.f * 0.1f;
    TriangulationData data = triangulate(text, font);

    const int numVertices = data.vertices.size();
    const int numIndices = data.indices.size();

    struct Vertex {
        QVector3D position;
        QVector3D normal;
    };

    QVector<IndexType> internalIndices;
    QVector<Vertex> internalVertices;

    float mx = FLT_MAX, Mx = -FLT_MAX, my = FLT_MAX, My = -FLT_MAX;

    for (QVector3D &v : data.vertices) {
        mx = v.x() < mx ? v.x() : mx;
        my = v.y() < my ? v.y() : my;
        Mx = v.x() > Mx ? v.x() : Mx;
        My = v.y() > My ? v.y() : My;
    }

    for (QVector3D &v : data.vertices) {
        v.setX(v.x() - (Mx - mx) * 0.5f);
        v.setY(v.y() - (My - my) * 0.5f);
        v = v / (My - my);
    }

    for (QVector3D &v : data.vertices) {
        v = transform * v;
    }

    // TODO: keep 'vertices.size()' small when extruding
    internalVertices.reserve(data.vertices.size() * 2);
    for (QVector3D &v : data.vertices) { // front face
        QVector3D outIntersectionPoint;
        QVector3D minimumLength(FLT_MAX, FLT_MAX, FLT_MAX);

        bool isIntersected = false;

        for (int i = 0; i < originalVerticesCount / 3; ++i) {
            bool isIntersectedNow = false;

            auto vertex = v;
            auto v0 = originalVertices[3 * i + 0];
            auto v1 = originalVertices[3 * i + 1];
            auto v2 = originalVertices[3 * i + 2];

            isIntersectedNow = RayIntersectsTriangle(vertex - normalVector * 0.5f, normalVector,
                                      v0, v1, v2,
                                      outIntersectionPoint);
            if (isIntersectedNow) {
                qDebug() << "isIntersected:" << v << outIntersectionPoint;

                if ((v - outIntersectionPoint).lengthSquared() < (v - minimumLength).lengthSquared()) {
                    minimumLength = outIntersectionPoint;
                }

                isIntersected = true;
            }
        }

        if (std::isinf(minimumLength.z())) {
            isIntersected = false;
        }

        if (isIntersected) {
            internalVertices.push_back({ minimumLength, // vertex
                                 -normalVector }); // normal

            qDebug() << "isIntersected:" << v << minimumLength;
        } else {
            internalVertices.push_back({ v - normalVector, // vertex
                                 -normalVector }); // normal
        }
    }

    for (QVector3D &v : data.vertices) { // front face
        internalVertices.push_back({ v + normalVector * 0.2f, // vertex
                             normalVector }); // normal
    }

    for (auto& v : internalVertices) {
        qDebug() << "2:" << v.position;
    }

    for (int i = 0, verticesIndex = internalVertices.size(); i < data.outlines.size(); ++i) {
        const int begin = data.outlines[i].begin;
        const int end = data.outlines[i].end;
        const int verticesIndexBegin = verticesIndex;

        if (begin == end)
            continue;

        QVector3D prevNormal = QVector3D::crossProduct(
                    internalVertices[data.outlineIndices[end - 1] + numVertices].position - internalVertices[data.outlineIndices[end - 1]].position,
                internalVertices[data.outlineIndices[begin]].position - internalVertices[data.outlineIndices[end - 1]].position).normalized();

        for (int j = begin; j < end; ++j) {
            const bool isLastIndex = (j == end - 1);
            const IndexType cur = data.outlineIndices[j];
            const IndexType next = data.outlineIndices[((j - begin + 1) % (end - begin)) + begin]; // normalize, bring in range and adjust
            const QVector3D normal = QVector3D::crossProduct(internalVertices[cur + numVertices].position - internalVertices[cur].position, internalVertices[next].position - internalVertices[cur].position).normalized();

            // use smooth normals in case of a short angle
            const bool smooth = QVector3D::dotProduct(prevNormal, normal) > (90.0f - edgeSplitAngle) / 90.0f;
            const QVector3D resultNormal = smooth ? mix(prevNormal, normal, 0.5f) : normal;
            if (!smooth)             {
                internalVertices.push_back({internalVertices[cur].position,               prevNormal});
                internalVertices.push_back({internalVertices[cur + numVertices].position, prevNormal});
                verticesIndex += 2;
            }

            internalVertices.push_back({internalVertices[cur].position,               resultNormal});
            internalVertices.push_back({internalVertices[cur + numVertices].position, resultNormal});

            const int v0 = verticesIndex;
            const int v1 = verticesIndex + 1;
            const int v2 = isLastIndex ? verticesIndexBegin     : verticesIndex + 2;
            const int v3 = isLastIndex ? verticesIndexBegin + 1 : verticesIndex + 3;

            internalIndices.push_back(v0);
            internalIndices.push_back(v1);
            internalIndices.push_back(v2);
            internalIndices.push_back(v2);
            internalIndices.push_back(v1);
            internalIndices.push_back(v3);

            verticesIndex += 2;
            prevNormal = normal;
        }
    }

    // resize for following insertions
    const int indicesOffset = internalIndices.size();
    internalIndices.resize(internalIndices.size() + numIndices * 2);

    // copy values for back faces
    IndexType *indicesFaces = internalIndices.data() + indicesOffset;
    memcpy(indicesFaces, data.indices.data(), numIndices * sizeof(IndexType));

    // insert values for front face and flip triangles
    for (int j = 0; j < numIndices; j += 3)
    {
        indicesFaces[numIndices + j    ] = indicesFaces[j    ] + numVertices;
        indicesFaces[numIndices + j + 1] = indicesFaces[j + 2] + numVertices;
        indicesFaces[numIndices + j + 2] = indicesFaces[j + 1] + numVertices;
    }

    *vertices = new QVector3D[internalVertices.size() * 2];
    memcpy(*vertices, internalVertices.data(), sizeof(QVector3D) * internalVertices.size() * 2);
    *verticesSize = internalVertices.size();

    *indices = new unsigned int[internalIndices.size()];

    // Change CCW to CW
    for (int i = 0; i < internalIndices.size() / 3; ++i) {
        (*indices)[3*i + 0] = internalIndices[3*i + 0];
        (*indices)[3*i + 1] = internalIndices[3*i + 2];
        (*indices)[3*i + 2] = internalIndices[3*i + 1];
    }
    *indicesSize = internalIndices.size();
}
