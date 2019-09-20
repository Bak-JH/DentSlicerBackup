#include "feature/text3dgenerator.h"

using namespace Hix::Labelling;

Hix::Labelling::Text3D::Text3D(Qt3DCore::QEntity* parent)
	:SceneEntityWithMaterial(parent)
{
}

Hix::Labelling::Text3D::Text3D(Qt3DCore::QEntity* parent, Text3D& from)
{
	setParent(parent);
	translation = from.translation;
	normal = from.normal;
	text = from.text;
	fontName = from.fontName;
	fontWeight = from.fontWeight;
	fontSize = from.fontSize;
}

void Text3D::generateText3D(QFont font, QString text, Hix::Engine3D::Mesh* targetMesh,
							QVector3D targetNormal, float scale)
{
	qDebug() << "Text3D::generateText3D()";
	qDebug() << "FONT: " << font << "TEXT: " << text << "MESH: " << targetMesh;
	qDebug() << "POS: " << translation << "NOR: " << targetNormal << "SCALE: " << scale;
	painterPath.setFillRule(Qt::WindingFill);
	painterPath.addText(0, 0, font, text);
	QList<QPolygonF> polygons = painterPath.toSubpathPolygons(QTransform().scale(1.0f, -1.0f));
	
	for (auto polygon : polygons)
	{
		Path IntPath;
		for (auto point : polygon)
		{
			IntPath.push_back(toInt2DPt(QVector3D(point)));
		}
		IntPaths.emplace_back(IntPath);
	}

	clpr.AddPaths(IntPaths, ptSubject, true);

	// generate polytree
	clpr.Execute(ctUnion, polytree, pftNonZero, pftNonZero);
	
	PolytreeCDT polycdt(&polytree);
	_trigMap = polycdt.triangulate();

	//for (auto node : _trigMap)
	//{
	//	for (auto trig : node.second)
	//	{
	//		targetMesh->addFace(
	//			QVector3D(trig[0].x(), hitPoint.y(), trig[0].y()),
	//			QVector3D(trig[1].x(), hitPoint.y(), trig[1].y()),
	//			QVector3D(trig[2].x(), hitPoint.y(), trig[2].y())
	//		);
	//	}
	//}

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
		path.emplace_back(QVector3D(translation.x(), translation.y(), translation.z()));
		path.emplace_back(QVector3D(translation.x(), translation.y()-0.05f, translation.z()));


		std::vector<float> scales;
		scales.emplace_back(scale);
		scales.emplace_back(scale);
		scales.emplace_back(scale);
		extrudeAlongPath(targetMesh, QVector3D(0,-1,0), contour, path, jointContours, &scales);

		contour.clear();
		++i;
	}
	
	qDebug() << jointContours.front().front().y() << jointContours.back().front().y();
	for (auto node : _trigMap)
	{
		for (auto trig : node.second)
		{
			targetMesh->addFace(
				QVector3D(translation.x() + (trig[0].x() * scale), jointContours.back().front().y(), translation.z() + (trig[0].y() * scale)),
				QVector3D(translation.x() + (trig[1].x() * scale), jointContours.back().front().y(), translation.z() + (trig[1].y() * scale)),
				QVector3D(translation.x() + (trig[2].x() * scale), jointContours.back().front().y(), translation.z() + (trig[2].y() * scale))
			);

			targetMesh->addFace(
				QVector3D(translation.x() + (trig[2].x() * scale), jointContours.front().front().y(), translation.z() + (trig[2].y() * scale)),
				QVector3D(translation.x() + (trig[1].x() * scale), jointContours.front().front().y(), translation.z() + (trig[1].y() * scale)),
				QVector3D(translation.x() + (trig[0].x() * scale), jointContours.front().front().y(), translation.z() + (trig[0].y() * scale))
			);
		}
	}

}

void Hix::Labelling::Text3D::setTranslation(QVector3D t)
{
	translation = t;
}

QVector3D Text3D::getPrimitiveColorCode(const Hix::Engine3D::Mesh* mesh, Hix::Engine3D::FaceConstItr faceItr)
{
	throw std::runtime_error("Text3D::getPrimitiveColorCode not implemented");
	return QVector3D();
}
