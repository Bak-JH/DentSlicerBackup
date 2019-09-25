#include "labelModel.h"
#include "utils/mathutils.h"

Hix::Labelling::LabelModel::LabelModel(Qt3DCore::QEntity* parent)
	:SceneEntityWithMaterial(parent)
{
}

Hix::Labelling::LabelModel::LabelModel(Qt3DCore::QEntity* parent, LabelModel& from)
{
	setParent(parent);
	translation = from.translation;
	normal = from.normal;
	text = from.text;
	font = from.font;
}

void Hix::Labelling::LabelModel::generateLabel(QString text, Hix::Engine3D::Mesh* targetMesh,
												QVector3D targetNormal, float scale)
{
	qDebug() << "Text3D::generateText3D()";
	qDebug() << "FONT: " << font << "TEXT: " << text << "MESH: " << targetMesh;
	qDebug() << "POS: " << translation << "NOR: " << targetNormal << "SCALE: " << scale;
	QPainterPath painterPath;
	painterPath.setFillRule(Qt::WindingFill);
	painterPath.addText(0, 0, font, text);

	auto width = painterPath.boundingRect().width();
	auto height = painterPath.boundingRect().height();

	qDebug() << "WIDTH: " << width << "HEIGHT: " << height;

	// translate float points to int point
	QList<QPolygonF> polygons = painterPath.toSubpathPolygons(QTransform().scale(1.0f, -1.0f));
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
		path.emplace_back(QVector3D(translation.x(), translation.y(), translation.z()));
		path.emplace_back(QVector3D(translation.x(), translation.y()-0.05f, translation.z()));


		std::vector<float> scales;
		scales.emplace_back(scale);
		scales.emplace_back(scale);
		scales.emplace_back(scale);
		extrudeAlongPath(_mesh, QVector3D(0,-1,0), contour, path, jointContours, &scales);

		contour.clear();
		++i;
	}
	

	QQuaternion test = QQuaternion::rotationTo(QVector3D(0, -1, 0), targetNormal);
	qDebug() << "NORMAL: " << targetNormal << "QUAT: " << test;
	_mesh->vertexRotate(Utils::Math::quatToMat(QQuaternion::rotationTo(QVector3D(0, -1, 0), targetNormal)));

	// generate front & back mesh
	for (auto node : _trigMap)
	{
		for (auto trig : node.second)
		{
			_mesh->addFace(
				QVector3D(translation.x() + (trig[0].x() * scale), jointContours.back().front().y(), translation.z() + (trig[0].y() * scale)),
				QVector3D(translation.x() + (trig[1].x() * scale), jointContours.back().front().y(), translation.z() + (trig[1].y() * scale)),
				QVector3D(translation.x() + (trig[2].x() * scale), jointContours.back().front().y(), translation.z() + (trig[2].y() * scale))
			);

			_mesh->addFace(
				QVector3D(translation.x() + (trig[2].x() * scale), jointContours.front().front().y(), translation.z() + (trig[2].y() * scale)),
				QVector3D(translation.x() + (trig[1].x() * scale), jointContours.front().front().y(), translation.z() + (trig[1].y() * scale)),
				QVector3D(translation.x() + (trig[0].x() * scale), jointContours.front().front().y(), translation.z() + (trig[0].y() * scale))
			);
		}
	}

	setMesh(_mesh);
}

void Hix::Labelling::LabelModel::setTranslation(QVector3D t)
{
	translation = t;
}

void Hix::Labelling::LabelModel::clicked(Hix::Input::MouseEventData&, const Qt3DRender::QRayCasterHit&)
{
}

void Hix::Labelling::LabelModel::onEntered()
{
	setHighlight(true);
}

void Hix::Labelling::LabelModel::onExited()
{
	setHighlight(false);
}

void Hix::Labelling::LabelModel::setHighlight(bool enable)
{
	auto color = Hix::Render::Colors::Support;
	if (enable)
	{
		color = Hix::Render::Colors::SupportHighlighted;
	}
	_meshMaterial.setColor(color);
}

QVector3D Hix::Labelling::LabelModel::getPrimitiveColorCode(const Hix::Engine3D::Mesh* mesh, Hix::Engine3D::FaceConstItr faceItr)
{
	throw std::runtime_error("Text3D::getPrimitiveColorCode not implemented");
	return QVector3D();
}

void Hix::Labelling::LabelModel::initHitTest()
{
	addComponent(&_layer);
	_layer.setRecursive(false);

}