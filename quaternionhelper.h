#ifndef QUATERNIONHELPER_H
#define QUATERNIONHELPER_H

#include <QObject>
#include <QDebug>

#include "qquaternion.h"

class QuaternionHelper : public QObject
{
    Q_OBJECT
public:
    explicit QuaternionHelper(QObject *parent = nullptr);

    Q_INVOKABLE void test();
    Q_INVOKABLE QQuaternion multiplyQuaternion(QQuaternion a, QQuaternion b);
    Q_INVOKABLE QVector3D rotatedVector(QQuaternion q, QVector3D v);
signals:

public slots:
};

#endif // QUATERNIONHELPER_H
