#ifndef MODELLOADER_H
#define MODELLOADER_H
#include <QObject>
#include <QDebug>
#include <QDateTime>
#include <QVector>
#include <QVector3D>
#include <QDataStream>
#include <QFile>

class face : public QObject
{
    Q_OBJECT
public:
    explicit face(QObject *parent = 0);
private:
    QVector3D normalVector;
    QVector<QVector3D> triAngle;
public:

    QVector3D getNormalVector() const;
    void setNormalVector(const QVector3D &value);

    QVector<QVector3D> getTriAngle() const;
    void setTriAngle(const QVector<QVector3D> &value);

signals:

public slots:

};

typedef struct modelSize{
    float x_min;
    float x_max;
    float y_min;
    float y_max;
    float z_min;
    float z_max;
} modelSize;

class ModelLoader : public QObject
{
    Q_OBJECT
public:
    explicit ModelLoader(QObject *parent = 0);
private:
    QList<face*> faceList;
    modelSize size;
    QVector3D center;
public:
    void loadModel(QString path);
    void readSTLA(QString path);
    void readSTLB(QString path);
    void clearModel();

    void model_test();
    modelSize getSize();
    void updateMinMax(QVector3D point);
    QVector3D getCenter(modelSize size);

    int getFacesCount();
private:
    QVector3D getCoordinateFromString(QString line);
signals:
    void signal_finishLoad();

public slots:

};


#endif // MODELLOADER_H
