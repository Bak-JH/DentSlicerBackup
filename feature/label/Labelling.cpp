#include "Labelling.h"
#include "qmlmanager.h"
#include "../Shapes2D.h"
#include "../Extrude.h"
using namespace Hix::Engine3D;
using namespace Hix::Polyclipping;
using namespace Hix::Shapes2D;
using namespace Qt3DCore;
using namespace Hix::Features::Extrusion;
using namespace ClipperLib;

//Mesh* generateLabelMesh(const QVector3D translation, const QVector3D normal, const QString text, const QFont font)

GLModel* Hix::Features::LabellingMode::generatePreviewModel()
{

	auto labelMesh = new Mesh();
	QPainterPath painterPath;
	painterPath.setFillRule(Qt::WindingFill);
	painterPath.addText(0, 0, _font, _text);

	auto width = painterPath.boundingRect().width();
	auto height = painterPath.boundingRect().height();
	// translate float points to int point
	QList<QPolygonF> polygons = painterPath.toSubpathPolygons(::QTransform().scale(1.0f, -1.0f));
	std::vector<Path> IntPaths;
	std::vector<std::vector<QVector3D>> fPath;

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
	std::vector<std::vector<QVector3D>> jointContours;
	std::vector<QVector3D> path;
	path.emplace_back(0, 0, -10);
	path.emplace_back(0, 0, 5);
	for (auto& intPath : IntPaths)
	{
		std::vector<QVector3D> contour;
		contour.reserve(intPath.size());
		for (auto& point : intPath)
		{
			contour.emplace_back(QVector3D(toFloatPt(point).x(), toFloatPt(point).y(), 0));
		}
		std::reverse(contour.begin(), contour.end());

		extrudeAlongPath<int>(labelMesh, QVector3D(0, 0, 1), contour, path, jointContours);

		contour.clear();
	}

	//generate front & back mesh
	for (auto node : _trigMap)
	{
		for (auto trig : node.second)
		{
			labelMesh->addFace(
				QVector3D(trig[0].x(), trig[0].y(), path.back().z()),
				QVector3D(trig[1].x(), trig[1].y(), path.back().z()),
				QVector3D(trig[2].x(), trig[2].y(), path.back().z())
			);

			labelMesh->addFace(
				QVector3D(trig[2].x(), trig[2].y(), path.front().z()),
				QVector3D(trig[1].x(), trig[1].y(), path.front().z()),
				QVector3D(trig[0].x(), trig[0].y(), path.front().z()));
		}
	}

	return new GLModel(_targetModel, labelMesh, "label", 0);
}

Hix::Features::LabellingMode::LabellingMode()
{
}



Hix::Features::LabellingMode::~LabellingMode()
{
}

void Hix::Features::LabellingMode::faceSelected(GLModel* selected, const Hix::Engine3D::FaceConstItr& selectedFace, const Hix::Input::MouseEventData& mouse, const Qt3DRender::QRayCasterHit& hit)
{
	_targetModel = selected;
	updateLabelMesh(hit.localIntersection(), selectedFace);

}
//void xyPlanePath(const QVector3D& worldStart, const Hix::Engine3D::FaceConstItr& face, float width)
//{
//	auto radius = width / 2;
//	auto worldZ = worldStart.z();
//	auto nFaces = face.neighborFaces();
//	std::unordered_set<FaceConstItr> explored;
//	explored.insert(face);
//	std::deque<FaceConstItr> qL;
//	std::deque<FaceConstItr> qR;
//	for (auto& nf : nFaces)
//	{
//		auto sortedZ = nf.sortZ();
//		if (sortedZ[2] >= worldZ && sortedZ[0] <= worldZ)
//		{
//			//q
//		}
//	}
//}
void Hix::Features::LabellingMode::updateLabelMesh(const QVector3D& localIntersection, const Hix::Engine3D::FaceConstItr& face)
{
	//setMaterialColor(Hix::Render::Colors::Support);
	if (_isDirty)
	{
		_previewModel.reset(generatePreviewModel());
	}
	//auto worldIntersection = _targetModel->ptToRoot(localIntersection);
	auto worldZ = _targetModel->ptToRoot(localIntersection).z();
	auto width = _previewModel->aabb().lengthX();
	auto height = _previewModel->aabb().lengthY();
	//float zHeight = acos()
	//move model
	auto rotation = QQuaternion::fromDirection(face.worldFn(), QVector3D(0, 0, 1));
	Qt3DCore::QTransform worldTrans;
	worldTrans.setRotation(rotation);
	auto toLocalMat = _targetModel->toLocalMatrix();
	auto finalMat = toLocalMat * worldTrans.matrix();
	Qt3DCore::QTransform finalTrans;
	finalTrans.setMatrix(finalMat);
	Qt3DCore::QTransform newTransform;
	newTransform.setRotation(finalTrans.rotation());
	newTransform.setTranslation(localIntersection);
	newTransform.setScale(0.05f);
	_previewModel->transform().setMatrix(newTransform.matrix());
	_previewModel->updateAABBScale(newTransform.scale3D());
	_matrix = newTransform.matrix();
	_scale = newTransform.scale3D();
}

void Hix::Features::LabellingMode::setText(const QString& text)
{
	if (_text != text)
	{
		_text = text;
		_isDirty = true;
		_previewModel.reset(generatePreviewModel());
		_previewModel->transform().setMatrix(_matrix);
		_previewModel->updateAABBScale(_scale);
	}
}

void Hix::Features::LabellingMode::setFontName(const QString& fontName)
{
	_font.setFamily(fontName);
	_isDirty = true;
	_previewModel.reset(generatePreviewModel());
	_previewModel->transform().setMatrix(_matrix);
	_previewModel->updateAABBScale(_scale);
}

void Hix::Features::LabellingMode::setFontBold(bool isBold)
{
	if (_font.bold() != isBold)
	{
		_font.setBold(isBold);
		_isDirty = true;	
		_previewModel.reset(generatePreviewModel());
		_previewModel->transform().setMatrix(_matrix);
		_previewModel->updateAABBScale(_scale);
	}
}

void Hix::Features::LabellingMode::setFontSize(int fontSize)
{
	if (_font.pointSize()!= fontSize)
	{
		_font.setPointSize(fontSize);
		_isDirty = true;
		_previewModel.reset(generatePreviewModel());
		_previewModel->transform().setMatrix(_matrix);
		_previewModel->updateAABBScale(_scale);
	}
}

Hix::Features::Feature* Hix::Features::LabellingMode::applyLabelMesh()
{
	if (!_previewModel)
	{
		//qDebug() << "no labellingTextPreview";
		//QMetaObject::invokeMethod(qmlManager->labelPopup, "noModel");
		return nullptr;
	}

	return new Labelling(_targetModel, _previewModel.release());
}



Hix::Features::Labelling::Labelling(GLModel* parentModel, GLModel* previewModel)
	: _targetModel(parentModel), _label(previewModel)
{
}

Hix::Features::Labelling::~Labelling()
{
}

void Hix::Features::Labelling::undoImpl()
{
	_label->QNode::setParent((QNode*)nullptr);
}

void Hix::Features::Labelling::redoImpl()
{
	if (!qmlManager->isSelected(_targetModel))
		_label->setMaterialColor(Hix::Render::Colors::Default);
	_label->QNode::setParent(_targetModel);
}

void Hix::Features::Labelling::runImpl()
{
	_targetModel->setMaterialColor(Hix::Render::Colors::Selected);
	_targetModel->updateModelMesh();
}
