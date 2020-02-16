#include "PrintInfo.h"
#include <qvector3d.h>
Hix::QML::PrintInfo::PrintInfo(QQuickItem* parent)
{
	setParent(parent);
}

void Hix::QML::PrintInfo::printVolumeChanged(const QVector3D& bound)
{
	_printsize =
		QString::number(bound.x(), 'e', 2) + "mm x " +
		QString::number(bound.y(), 'e', 2) + "mm x " +
		QString::number(bound.z(), 'e', 2) + "mm";
	_printvolume = QString::number(bound.x() * bound.y() * bound.z(), 'e', 2) + "mL";

}
