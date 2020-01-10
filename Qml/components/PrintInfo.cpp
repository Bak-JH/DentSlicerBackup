#include "PrintInfo.h"

Hix::QML::PrintInfoText::PrintInfoText(QQuickItem* parent)
{
	setParent(parent);
}

void Hix::QML::PrintInfoText::printtimeChanged()
{
	qDebug() << _printtime;
}

void Hix::QML::PrintInfoText::printlayerChanged()
{
	qDebug() << _printlayer;
}

void Hix::QML::PrintInfoText::printsizeChanged()
{
	qDebug() << _printsize;
}

void Hix::QML::PrintInfoText::printvolumeChanged()
{
	qDebug() << _printvolume;
}