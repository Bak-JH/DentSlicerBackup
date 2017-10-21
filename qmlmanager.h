#ifndef QMLMANAGER_H
#define QMLMANAGER_H

#include <QObject>
#include <QDebug>
#include <QString>

class QmlManager : public QObject
{
    Q_OBJECT
public:
    explicit QmlManager(QObject *parent = nullptr);

signals:
    void updateModelInfo(int printing_time, int layer, QString xyz, float volume);

public slots:
    void sendUpdateModelInfo(int, int, QString, float);
};

extern QmlManager qm;

#endif // QMLMANAGER_H
