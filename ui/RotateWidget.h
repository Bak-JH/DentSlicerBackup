#pragma once

#include "glmodel.h"

using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Qt3DExtras;
class RotateWidget:public QEntity
{
	Q_OBJECT
public:

private:
	QMesh _widgetMesh;
	Qt3DCore::QTransform _widgetTransform;
};

