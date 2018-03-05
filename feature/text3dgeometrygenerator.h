#ifndef TEXT3DGEOMETRYGENERATOR_H
#define TEXT3DGEOMETRYGENERATOR_H

#include <QVector3D>
#include <QFont>
#include <QString>

void generateText3DGeometry(QVector3D** vertices, int* verticesSize,
                            unsigned int** indices, int* indicesSize,
                            QFont font, QString text, float depth,
                            const QVector3D* originalVertices,
                            const int originalVerticesCount,
                            const QVector3D normalVector,
                            const QMatrix4x4& transform,
                            const QMatrix4x4& normalTransform);

#endif // TEXT3DGEOMETRYGENERATOR_H
