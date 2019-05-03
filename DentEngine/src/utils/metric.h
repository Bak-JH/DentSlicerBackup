#ifndef METRIC_H
#define METRIC_H
#include "../polyclipping/clipper/clipper.hpp"
#include <QVector3D>

using namespace ClipperLib;

float pointDistance(IntPoint A, IntPoint B);
float pointDistance(QVector3D A, QVector3D B);

#endif // INTPOINT
