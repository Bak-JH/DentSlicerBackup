#ifndef TEXT3DGEOMETRYGENERATOR_H
#define TEXT3DGEOMETRYGENERATOR_H

#include <QVector3D>
#include <QFont>
#include <QString>
#include <QPlaneMesh>
#include "DentEngine/src/mesh.h"

void generateText3DGeometry(QVector3D** vertices, int* verticesSize,
                            unsigned int** indices, int* indicesSize,
                            QFont font, QString text, float depth,
                            Hix::Engine3D::Mesh* mesh,
                            //const QVector3D* originalVertices,
                            //const int originalVerticesCount,
                            const QVector3D normalVector,
                            const QMatrix4x4& transform,
                            const QMatrix4x4& normalTransform);

#endif // TEXT3DGEOMETRYGENERATOR_H
