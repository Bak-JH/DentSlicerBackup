#pragma once
#include <QPainter>
#include <deque>
#include <random>
#include <QSvgGenerator>
#include <string>
#include "ContourBuilder.h"
#include "SlicerDebug.h"

namespace Hix
{
	namespace Slicer
	{
		namespace Debug
		{
			template<class ContainerType>
			void drawContour(const ContainerType& input, QPainter& painter, QString contourName)
			{
				QPainterPath path;
				painter.drawText(QPoint(input.front().x(), input.front().y()), contourName);
				path.moveTo(input.front().x(), input.front().y());
				for (auto& each : input)
				{
					path.lineTo(each.x(), each.y());
				}
				painter.strokePath(path, painter.pen());
			}

			class SVGOut
			{
			public:
				SVGOut(const std::string& name, const QSize& size);
				~SVGOut();
				QColor randColor();
				QPen defaultPen(const QColor& color);
				QPen defaultDottedPen(const QColor& color);
				

				template<class ContainerType>
				void addPath(const ContainerType& cont, QPen pen, const std::string& name)
				{
					_painter.setPen(pen);
					auto contourName = QString::fromStdString(name);
					drawContour(cont, _painter, contourName);
				}
			private:
				std::mt19937 _randGen;
				QPainter _painter;
				QSvgGenerator _generator;
				std::uniform_int_distribution<short> _randColor;


			};

			void outDebugSVGs(const std::deque<Hix::Slicer::Contour>& contours, size_t z);
			void outDebugIncompletePathsSVGs(const std::deque<Hix::Slicer::Contour>& contours, size_t z);

		}
	}
}

