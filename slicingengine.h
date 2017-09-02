#ifndef SLICINGENGINE_H
#define SLICINGENGINE_H

#include <QObject>
#include <QDebug>
#include <QString>
#include <QProcess>
#include "configuration.h"

class SlicingEngine : public QObject
{
    Q_OBJECT
public:
    SlicingEngine();
    Q_INVOKABLE void slice (QVariantMap cfg);
};

#endif // SLICINGENGINE_H
