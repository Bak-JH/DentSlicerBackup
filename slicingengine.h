#ifndef SLICINGENGINE_H
#define SLICINGENGINE_H

#include <QObject>
#include <QDebug>
#include <QString>
#include <QProcess>
#include "feature/stlexporter.h"
#include "DentEngine/src/slicer.h"
#include "DentEngine/src/configuration.h"
#include "DentEngine/src/svgexporter.h"

class SlicingEngine : public QObject
{
    Q_OBJECT
public:
    SlicingEngine();
    Q_INVOKABLE Slicer* slice (QVariant cfg, Mesh* mesh=nullptr, QString filename = "");

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
