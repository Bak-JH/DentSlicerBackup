#include "SlicerDebugInfoExport.h"
#include "SlicerDebug.h"
#include <QSvgGenerator>
#include "../../application/ApplicationManager.h"

void drawContour(const Hix::Slicer::Contour& contour, QPainter& painter, QString contourName)
{
	auto intPath = contour.toDebugPath();
	QPainterPath path;
	painter.drawText(QPoint(intPath.front().X, intPath.front().Y), contourName);
	path.moveTo(intPath.front().X, intPath.front().Y);
	for (auto& each : intPath)
	{
		path.lineTo(each.X, each.Y);
	}
	painter.strokePath(path, painter.pen());
}
void Hix::Slicer::Debug::outDebugSVGs(const std::deque<Hix::Slicer::Contour>& contours, size_t z)
{
	//random gen for random color
	std::random_device rd;  
	std::mt19937 gen(rd()); 
	std::uniform_int_distribution<short> randColor(0, 255);
	
	//svg file name
	QString newPath = SlicerDebug::getInstance().debugFilePath + QString::number(z) + QString(".svg");
	auto& pSet = Hix::Application::ApplicationManager::getInstance().settings().printerSetting;
	QSvgGenerator generator;
	generator.setFileName(newPath);
	QSize size(pSet.sliceImageResolutionX + 200, pSet.sliceImageResolutionY + 200);
	generator.setSize(size);
	generator.setViewBox(QRect(0, 0, size.width(), size.height()));
	QPainter painter;
	painter.begin(&generator);    
	painter.setFont(QFont("Arial", 20));
	size_t count = 0;
	painter.fillRect(QRect(0,0,size.width(), size.height()), Qt::GlobalColor::black);
	for (auto& contour : contours)
	{
		//randomize color for each contour
		auto r = randColor(gen);
		auto g = randColor(gen);
		auto b = randColor(gen);
		auto penType = Qt::SolidLine;
		if (!contour.isOutward())
		{
			penType = Qt::DashLine;
		}
		painter.setPen(QPen(QColor::fromRgb(r,g,b),4, penType));
		auto contourName = QString::number(count);
		drawContour(contour, painter, contourName);
		++count;
	}
	painter.end();

}



void Hix::Slicer::Debug::outDebugIncompletePathsSVGs(const std::deque<Hix::Slicer::Contour>& contours, size_t z)
{
	//random gen for random color
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<short> randColor(0, 255);

	//svg file name
	QString newPath = SlicerDebug::getInstance().debugFilePath +"Incomplete_"+ QString::number(z) + QString(".svg");
	auto& pSet = Hix::Application::ApplicationManager::getInstance().settings().printerSetting;

	QSvgGenerator generator;
	generator.setFileName(newPath);
	QSize size(pSet.sliceImageResolutionX + 200, pSet.sliceImageResolutionY + 200);
	generator.setSize(size);
	generator.setViewBox(QRect(0, 0, size.width(), size.height()));
	QPainter painter;
	painter.begin(&generator);
	painter.setFont(QFont("Arial", 20));
	size_t count = 0;
	painter.fillRect(QRect(0, 0, size.width(), size.height()), Qt::GlobalColor::black);
	for (auto& contour : contours)
	{
		//randomize color for each contour
		auto r = randColor(gen);
		auto g = randColor(gen);
		auto b = randColor(gen);
		auto penType = Qt::SolidLine;
		painter.setPen(QPen(QColor::fromRgb(r, g, b), 4, penType));
		auto contourName = QString::number(count);
		drawContour(contour, painter, contourName);
		++count;
	}
	painter.end();

}
