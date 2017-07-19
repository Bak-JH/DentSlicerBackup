#include "quaternionhelper.h"


QuaternionHelper::QuaternionHelper(QObject *parent) : QObject(parent)
{

}

void QuaternionHelper::test()
{
    qDebug() << "Hello Test";
}


QQuaternion QuaternionHelper::multiplyQuaternion(QQuaternion a, QQuaternion b)
{
    return a * b;
}

QVector3D QuaternionHelper::rotatedVector(QQuaternion q, QVector3D v)
{
    return q.rotatedVector(v);
}
