#ifndef MODELCUT_H
#define MODELCUT_H

#include <QtCore/QObject>
#include <Qt3DCore/qentity.h>
#include <Qt3DCore/qtransform.h>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QPlaneMesh>
#include <QtMath>
#include <QVector3D>
#include <Qt3DRender/qpointlight.h>
#include <QString>


class Lights: public QObject
{
    Q_OBJECT

public:
    explicit Lights(Qt3DCore::QEntity *rootEntity);
    ~Lights();

private:
    Qt3DCore::QEntity *motherEntity;
    Qt3DCore::QEntity *lightEntity[4];
    Qt3DRender::QPointLight *light[4];
    Qt3DCore::QTransform *lightTransform[4];
    int light_distance = 100;
    int number_of_lights = 4;
};

#endif // MODELCUT_H
