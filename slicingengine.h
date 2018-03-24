#ifndef SLICINGENGINE_H
#define SLICINGENGINE_H

#include <QObject>
#include <QDebug>
#include <QString>
#include <QProcess>
#include "configuration.h"
#include "qmlmanager.h"
#include "feature/stlexporter.h"

class SlicingEngine : public QObject
{
    Q_OBJECT
public:
    SlicingEngine();
    Q_INVOKABLE void slice (QVariantMap cfg);

public slots:
    void slicingStarted();
    void slicingOutput();
    void slicingFinished(int, QProcess::ExitStatus);
    void slicingError(QProcess::ProcessError);

signals:
    void updateModelInfo(int printing_time, int layer, QString xyz, float volume);
};

//extern QmlManager qm;

#endif // SLICINGENGINE_H
