#include <QGeometryRenderer>
#include <QSize>
#include <QVector3D>
#include "gridmesh.h"
#include "linemeshgeometry.h"

GridMesh::GridMesh(Qt3DCore::QNode *parent)
    : Qt3DRender::QGeometryRenderer(parent)
{
    setInstanceCount(1);
    setIndexOffset(0);
    setFirstInstance(0);
    setPrimitiveType(Qt3DRender::QGeometryRenderer::Lines);
    setMeshResolution(QSize(20, 20));
}

QSize GridMesh::meshResolution() const
{
    return m_meshResolution;
}

void GridMesh::setMeshResolution(const QSize &meshResolution)
{
    if (meshResolution == m_meshResolution) {
        return;
    }

    QVector<QVector3D> vertices;

    const float dx = 1.0f / static_cast<float>(meshResolution.width());
    for (int col = 0; col <= meshResolution.width(); col++) {
        const float x = col * dx;
        vertices.append(QVector3D(x, 0.0f, 0.0f));
        vertices.append(QVector3D(x, 1.0f, 0.0f));
    }

    const float dy = 1.0f / static_cast<float>(meshResolution.height());
    for (int row = 0; row <= meshResolution.height(); row++) {
        const float y = row * dy;
        vertices.append(QVector3D(0.0f, y, 0.0f));
        vertices.append(QVector3D(1.0f, y, 0.0f));
    }
    vertices.append(QVector3D(0.0f, 0.0f, 0.0f));
    vertices.append(QVector3D(0.0f, 0.0f, 100.0f));

    vertices.append(QVector3D(0.0f, 1.0f, 0.0f));
    vertices.append(QVector3D(0.0f, 1.0f, 100.0f));

    vertices.append(QVector3D(1.0f, 0.0f, 0.0f));
    vertices.append(QVector3D(1.0f, 0.0f, 100.0f));

    vertices.append(QVector3D(1.0f, 1.0f, 0.0f));
    vertices.append(QVector3D(1.0f, 1.0f, 100.0f));

    if (geometry()) {
        auto geom = geometry();
        setGeometry(nullptr);
        delete geom;
    }

    auto geom = new LineMeshGeometry(vertices, this);
    setVertexCount(geom->vertexCount());
    setGeometry(geom);

    m_meshResolution = meshResolution;
    emit meshResolutionChanged(m_meshResolution);
}
