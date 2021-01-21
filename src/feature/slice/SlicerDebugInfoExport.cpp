#include "SlicerDebugInfoExport.h"
#include "../../application/ApplicationManager.h"


Hix::Slicer::Debug::SVGOut::SVGOut(const std::string& name, const QSize& size): _randGen(1), _randColor(0, 255)
{
	QString newPath = SlicerDebug::getInstance().debugFilePath + QString::fromStdString(name) + QString(".svg");
	auto& pSet = Hix::Application::ApplicationManager::getInstance().settings().printerSetting;
	_generator.setFileName(newPath);
	_generator.setSize(size);
	_generator.setViewBox(QRect(0, 0, size.width(), size.height()));
	_painter.begin(&_generator);
	_painter.setFont(QFont("Arial", size.width()/1000));
	_painter.fillRect(QRect(0, 0, size.width(), size.height()), Qt::GlobalColor::black);


}

Hix::Slicer::Debug::SVGOut::~SVGOut()
{
	_painter.end();
}

QColor Hix::Slicer::Debug::SVGOut::randColor()
{
	return QColor(_randColor(_randGen), _randColor(_randGen), _randColor(_randGen));
}

QPen Hix::Slicer::Debug::SVGOut::defaultPen(const QColor& color)
{
	return QPen(color, 4, Qt::SolidLine);
}

QPen Hix::Slicer::Debug::SVGOut::defaultDottedPen(const QColor& color)
{
	return QPen(color, 4, Qt::DashLine);
}


//void drawContour(const Hix::Slicer::Contour& contour, QPainter& painter, QString contourName)
//{
//	auto intPath = contour.toDebugPath();
//	QPainterPath path;
//	painter.drawText(QPoint(intPath.front().X, intPath.front().Y), contourName);
//	path.moveTo(intPath.front().X, intPath.front().Y);
//	for (auto& each : intPath)
//	{
//		path.lineTo(each.X, each.Y);
//	}
//	painter.strokePath(path, painter.pen());
//}

void Hix::Slicer::Debug::outDebugSVGs(const Hix::Slicer::LayerGroup& layer, size_t z)
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
	painter.fillRect(QRect(0, 0, size.width(), size.height()), Qt::GlobalColor::black);
	for (auto& slc : layer.slices)
	{
		auto& contours= slc.closedContours;
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
			painter.setPen(QPen(QColor::fromRgb(r, g, b), 4, penType));
			auto contourName = QString::number(count);
			auto intPath = contour.toDebugPath();
			drawContour(intPath, painter, contourName);
			++count;
		}
	}
	painter.end();

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
		auto intPath = contour.toDebugPath();
		drawContour(intPath, painter, contourName);
		++count;
	}
	painter.end();

}

void Hix::Slicer::Debug::outDebugIncompletePathsSVGs(const Hix::Slicer::LayerGroup& layer, size_t z)
{
	//random gen for random color
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<short> randColor(0, 255);

	//svg file name
	QString newPath = SlicerDebug::getInstance().debugFilePath + "Incomplete_" + QString::number(z) + QString(".svg");
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
	for (auto& slc : layer.slices)
	{
		auto& contours = slc.incompleteContours;
		for (auto& contour : contours)
		{
			//randomize color for each contour
			auto r = randColor(gen);
			auto g = randColor(gen);
			auto b = randColor(gen);
			auto penType = Qt::SolidLine;
			painter.setPen(QPen(QColor::fromRgb(r, g, b), 4, penType));
			auto contourName = QString::number(count);
			auto intPath = contour.toDebugPath();
			drawContour(intPath, painter, contourName);
			++count;
		}
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
		auto intPath = contour.toDebugPath();
		drawContour(intPath, painter, contourName);
		++count;
	}
	painter.end();

}
