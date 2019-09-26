

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
private:
    Qt3DCore::QEntity *parentEntity;
    int light_distance = 100;
};
