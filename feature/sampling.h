#pragma once
#include <QVector3D>
#include "Mesh/mesh.h"

QVector3D getClosestVertex(const QVector3D point, Hix::Engine3D::FaceConstItr face);