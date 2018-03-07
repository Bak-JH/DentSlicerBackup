#ifndef QMLMANAGER_H
#define QMLMANAGER_H

#include <QObject>
#include <QDebug>
#include <QString>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlEngine>

class QmlManager : public QObject
{
    Q_OBJECT
public:
    explicit QmlManager(QObject *parent = nullptr);
    QQmlApplicationEngine* engine;

    void initializeUI(QQmlApplicationEngine *e);

signals:
    void updateModelInfo(int printing_time, int layer, QString xyz, float volume);

public slots:
    void sendUpdateModelInfo(int, int, QString, float);
};



QObject* FindItemByName(QList<QObject*> nodes, const QString& name);
QObject* FindItemByName(QQmlApplicationEngine* engine, const QString& name);



extern QmlManager qm;

#endif // QMLMANAGER_H
