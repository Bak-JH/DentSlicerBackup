#ifndef LINEMESHGEOMETRY_H
#define LINEMESHGEOMETRY_H

#include <QAttribute>
#include <QGeometry>
#include <Qt3DRender/QBuffer>

class LineMeshGeometry : public Qt3DRender::QGeometry
{
    Q_OBJECT

public:
    LineMeshGeometry(const QVector<QVector3D> &vertices, Qt3DCore::QNode *parent = Q_NULLPTR);
    ~LineMeshGeometry() = default;
    int vertexCount();

private:
    Qt3DRender::QAttribute *_positionAttribute;
    Qt3DRender::QBuffer *_vertexBuffer;
};

#endif // LINEMESHGEOMETRY_H
