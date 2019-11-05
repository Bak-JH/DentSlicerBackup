#include "labelModel.h"
#include "utils/mathutils.h"
#include <QTransform>

Hix::LabelModel::LabelModel(Qt3DCore::QEntity* parent):GLModel(parent)
{
	setMaterialColor(Hix::Render::Colors::Support);
}

void Hix::LabelModel::generateLabel(QVector3D targetNormal)
{
	auto labelMesh = new Mesh();
	QPainterPath painterPath;
	painterPath.setFillRule(Qt::WindingFill);
	painterPath.addText(0, 0, font, text);

	auto width = painterPath.boundingRect().width();
	auto height = painterPath.boundingRect().height();

	qDebug() << "WIDTH: " << width << "HEIGHT: " << height;

	// translate float points to int point
	QList<QPolygonF> polygons = painterPath.toSubpathPolygons(::QTransform().scale(1.0f, -1.0f));
	std::vector<Path> IntPaths;
	for (auto polygon : polygons)
	{
		Path IntPath;
		for (auto point : polygon)
		{
			point.setX(point.x() - (width / 2));
			point.setY(point.y() - (height / 2));
			IntPath.push_back(toInt2DPt(QVector3D(point)));
		}
		IntPaths.emplace_back(IntPath);
	}

	// add paths into clipper
	Clipper clpr;
	clpr.AddPaths(IntPaths, ptSubject, true);

	// generate polytree
	PolyTree polytree;
	clpr.Execute(ctUnion, polytree, pftNonZero, pftNonZero);
	
	// triangulate
	PolytreeCDT polycdt(&polytree);
	std::unordered_map<PolyNode*, std::vector<PolytreeCDT::Triangle>> _trigMap;
	_trigMap = polycdt.triangulate();

	// generate cyliner wall
	std::vector<QVector3D> contour;
	contour.reserve(IntPaths.size());
	std::vector<std::vector<QVector3D>> jointContours;

	for (auto& intPath : IntPaths)
	{
		int i = 0;
		for (auto& point : intPath)
		{
			contour.emplace_back(QVector3D(toFloatPt(point).x(), i, toFloatPt(point).y()));
		}

		std::reverse(contour.begin(), contour.end());

		std::vector<QVector3D> path;
		path.emplace_back(0, 0, 0);
		path.emplace_back(0, -2.f, 0);

		extrudeAlongPath<int>(labelMesh, QVector3D(0,-1,0), contour, path, jointContours);

		contour.clear();
		++i;
	}
	
	// generate front & back mesh
	for (auto node : _trigMap)
	{
		for (auto trig : node.second)
		{
			labelMesh->addFace(
				QVector3D(trig[0].x(), jointContours.back().front().y(), trig[0].y()),
				QVector3D(trig[1].x(), jointContours.back().front().y(), trig[1].y()),
				QVector3D(trig[2].x(), jointContours.back().front().y(), trig[2].y())
			);

			labelMesh->addFace(
				QVector3D(trig[2].x(), jointContours.front().front().y(), trig[2].y()),
				QVector3D(trig[1].x(), jointContours.front().front().y(), trig[1].y()),
				QVector3D(trig[0].x(), jointContours.front().front().y(), trig[0].y()));
		}
	}

	transform().setRotation(QQuaternion::rotationTo(QVector3D(0,-1,0), targetNormal));
	transform().setScale(0.05f);

	setMesh(labelMesh);
}

void Hix::LabelModel::setTranslation(QVector3D t)
{
	transform().setTranslation(t);
}