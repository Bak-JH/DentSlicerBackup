#ifndef SLICINGENGINE_H
#define SLICINGENGINE_H

#include <QObject>
#include <QDebug>
#include <QString>
#include <QProcess>
#include "feature/stlexporter.h"
#include "fileloader.h"
#include "DentEngine/src/slicer.h"
#include "DentEngine/src/configuration.h"
#include "DentEngine/src/svgexporter.h"

using namespace Hix::Engine3D;
class SlicingEngine : public QObject
{
    Q_OBJECT

public:
    SlicingEngine();
    Q_INVOKABLE Slicer* slice (QVariant cfg, Mesh* shellMesh=nullptr, Mesh* supportMesh=nullptr, Mesh* raftMesh=nullptr, QString filename = "");

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
