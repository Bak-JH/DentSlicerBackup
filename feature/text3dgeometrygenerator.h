#ifndef TEXT3DGEOMETRYGENERATOR_H
#define TEXT3DGEOMETRYGENERATOR_H

#include <QVector3D>
#include <QFont>
#include <QString>

void generateText3DGeometry(QVector3D** vertices, int* verticesSize,
                            unsigned int** indices, int* indicesSize,
                            QFont font, QString text, float depth);

#endif // TEXT3DGEOMETRYGENERATOR_H
