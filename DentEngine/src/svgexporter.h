#ifndef SVGEXPORTER_H
#define SVGEXPORTER_H
#include <sstream>
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
namespace Hix
{
	namespace Slicer
	{
		class Slices;

	}
}

namespace SVGexporter
{
	using namespace Hix::Slicer;
    QString exportSVG(Slices& shellSlices, Slices& supportSlices, Slices& raftSlices, QString outfoldername, bool isTemp);
};

#endif // SVGEXPORTER_H
