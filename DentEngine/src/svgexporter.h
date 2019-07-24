#ifndef SVGEXPORTER_H
#define SVGEXPORTER_H
#include <fstream>
#include <QDir>
#include <QJsonObject>
#include <QJsonDocument>
#include <QSvgRenderer>
#include <QImage>
#include <QPainter>
//namespace ClipperLib
//{
//	typedef PolyNode;
//	struct IntPoint;
//
//}
class Slices;

namespace SVGexporter
{

    QString exportSVG(Slices& shellSlices, Slices& supportSlices, Slices& raftSlices, QString outfoldername, bool isTemp);
};

#endif // SVGEXPORTER_H
