#ifndef SLICINGENGINE_H
#define SLICINGENGINE_H

#include <QObject>
#include <QDebug>
#include <QString>
#include <QProcess>
#include "DentEngine/src/configuration.h"
#include "feature/stlexporter.h"
#include "DentEngine/src/mesh.h"

class SlicingEngine : public QObject
{
    Q_OBJECT
public:
    SlicingEngine();
    Q_INVOKABLE void slice (QString cfg, QString filename);

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
