#ifndef QTRIANGULATOR_P_H
#define QTRIANGULATOR_P_H
//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/qvector.h>
#include <QPainterPath>
#include <QTransform>

QT_BEGIN_NAMESPACE

class QVertexIndexVector
{
public:
    enum Type {
        UnsignedInt,
        UnsignedShort
    };

    inline Type type() const { return t; }

    inline void setDataUint(const QVector<quint32> &data)
    {
        t = UnsignedInt;
        indices32 = data;
    }

    inline void setDataUshort(const QVector<quint16> &data)
    {
        t = UnsignedShort;
        indices16 = data;
    }

    inline const void* data() const
    {
        if (t == UnsignedInt)
            return indices32.data();
        return indices16.data();
    }

    inline int size() const
    {
        if (t == UnsignedInt)
            return indices32.size();
        return indices16.size();
    }

    inline QVertexIndexVector &operator = (const QVertexIndexVector &other)
    {
        if (t == UnsignedInt)
            indices32 = other.indices32;
        else
            indices16 = other.indices16;

        t = other.t;
        return *this;
    }

private:

    Type t;
    QVector<quint32> indices32;
    QVector<quint16> indices16;
};

struct QTriangleSet
{
    inline QTriangleSet() { }
    inline QTriangleSet(const QTriangleSet &other) : vertices(other.vertices), indices(other.indices) { }
    QTriangleSet &operator = (const QTriangleSet &other) {vertices = other.vertices; indices = other.indices; return *this;}

    // The vertices of a triangle are given by: (x[i[n]], y[i[n]]), (x[j[n]], y[j[n]]), (x[k[n]], y[k[n]]), n = 0, 1, ...
    QVector<qreal> vertices; // [x[0], y[0], x[1], y[1], x[2], ...]
    QVertexIndexVector indices; // [i[0], j[0], k[0], i[1], j[1], k[1], i[2], ...]
};

struct QPolylineSet
{
    inline QPolylineSet() { }
    inline QPolylineSet(const QPolylineSet &other) : vertices(other.vertices), indices(other.indices) { }
    QPolylineSet &operator = (const QPolylineSet &other) {vertices = other.vertices; indices = other.indices; return *this;}

    QVector<qreal> vertices; // [x[0], y[0], x[1], y[1], x[2], ...]
    QVertexIndexVector indices; // End of polyline is marked with -1.
};

QTriangleSet qTriangulate(const QPainterPath &path, const QTransform &matrix,
                                       qreal lod, bool allowUintIndices);
QPolylineSet qPolyline(const QPainterPath &path, const QTransform &matrix,
                                    qreal lod, bool allowUintIndices);

QT_END_NAMESPACE

#endif // QTRIANGULATOR_P_H
