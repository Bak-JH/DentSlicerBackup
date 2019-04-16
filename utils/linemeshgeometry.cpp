#include <QVector>
#include <QVector3D>
#include "linemeshgeometry.h"

LineMeshGeometry::LineMeshGeometry(const QVector<QVector3D> &vertices, Qt3DCore::QNode *parent) :
    Qt3DRender::QGeometry(parent)
    , _positionAttribute(new Qt3DRender::QAttribute(this))
    , _vertexBuffer(new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer, this))
{
    QByteArray vertexBufferData;
    vertexBufferData.resize(vertices.size() * static_cast<int>(sizeof(QVector3D)));
    memcpy(vertexBufferData.data(), vertices.constData(), static_cast<size_t>(vertexBufferData.size()));
    _vertexBuffer->setData(vertexBufferData);

    _positionAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    _positionAttribute->setBuffer(_vertexBuffer);
    _positionAttribute->setDataType(Qt3DRender::QAttribute::Float);
    _positionAttribute->setDataSize(3);
    _positionAttribute->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());

    addAttribute(_positionAttribute);
}

int LineMeshGeometry::vertexCount()
{
    return _vertexBuffer->data().size() / static_cast<int>(sizeof(QVector3D));
}
