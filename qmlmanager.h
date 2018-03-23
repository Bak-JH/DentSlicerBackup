#ifndef QMLMANAGER_H
#define QMLMANAGER_H

#include <QObject>
#include <QDebug>
#include <QString>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlEngine>
#include "feature/autoarrange.h"
#include "glmodel.h"
#include "QtConcurrent/QtConcurrentRun"
#include "QFuture"

class QmlManager : public QObject
{
    Q_OBJECT
public:
    explicit QmlManager(QObject *parent = nullptr);
    QQmlApplicationEngine* engine;
    QObject* mainWindow;
    QEntity* models;
    vector<GLModel*> glmodels;

    void initializeUI(QQmlApplicationEngine *e);
    void openModelFile_internal(QString filename);

private:
    bool glmodels_arranged;

signals:
    void updateModelInfo(int printing_time, int layer, QString xyz, float volume);

public slots:
    void sendUpdateModelInfo(int, int, QString, float);
    void openModelFile(QString filename);
    void runArrange();
};



QObject* FindItemByName(QList<QObject*> nodes, const QString& name);
QObject* FindItemByName(QQmlApplicationEngine* engine, const QString& name);



extern QmlManager qm;

#endif // QMLMANAGER_H
