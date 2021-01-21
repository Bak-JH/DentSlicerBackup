#include "PrintInfo.h"
#include <qvector3d.h>
#include <stdio.h>
#include <math.h>   
Hix::QML::PrintInfo::PrintInfo(QQuickItem* parent)
{
	setParent(parent);
}

void Hix::QML::PrintInfo::printVolumeChanged(QVector3D bound)
{
	if(isinf(bound.x()))
		bound.setX(0);
	if (isinf(bound.y()))
		bound.setY(0);
	if (isinf(bound.z()))
		bound.setZ(0);

	_printsize =
		QString::number(bound.x(), 'f', 2) + "mm x " +
		QString::number(bound.y(), 'f', 2) + "mm x " +
		QString::number(bound.z(), 'f', 2) + "mm";
	emit printsizeChanged();
}
