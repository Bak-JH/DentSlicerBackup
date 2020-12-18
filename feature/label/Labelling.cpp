#include "Labelling.h"
#include "Qml/components/Inputs.h"
#include "Qml/components/Buttons.h"
#include <algorithm>

#include "../qml/components/ControlOwner.h"

#include "../Shapes2D.h"
#include "../Extrude.h"
#include "../../glmodel.h"
#include "../../application/ApplicationManager.h"
#include "../repair/meshrepair.h"
//engrave CSG
#include "../CSG/CSG.h"
#include "../cdt/HixCDT.h"
#include "../Mesh/BVH.h"
#include "../Mesh/MTRayCaster.h"
using namespace Hix::Engine3D;
using namespace Hix::Polyclipping;
using namespace Hix::Shapes2D;
using namespace Hix::Features::Extrusion;
using namespace ClipperLib;
using namespace Qt3DCore;
using namespace Hix::Application;
using namespace Hix::Features::CSG;

//Mesh* generateLabelMesh(const QVector3D translation, const QVector3D normal, const QString text, const QFont font)
const QUrl LABEL_POPUP_URL = QUrl("qrc:/Qml/FeaturePopup/PopupLabel.qml");
constexpr float LABEL_SCALE = 0.05f;
GLModel* Hix::Features::LabellingMode::generatePreviewModel(const QMatrix4x4& transformMat)
{
	auto labelMesh = new Mesh();
	QPainterPath painterPath;
	painterPath.setFillRule(Qt::WindingFill);
	QFont font(_fontStyle->getSelectedItem(), _fontSize->getValue());
	painterPath.addText(0, 0, font, QString::fromStdString(_inputText->getInputText()));

	auto width = painterPath.boundingRect().width();
	auto height = painterPath.boundingRect().height();
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
	
	auto depth = _labelDepth->getValue() / LABEL_SCALE;

	// triangulate
	Hix::CDT::PolytreeCDT polycdt(&polytree);
	auto trigMap = polycdt.triangulate();

	//raycast 
	constexpr float RAY_Z = 20;
	QVector4D rayDir4D(0, 0, -1, 0);
	rayDir4D = transformMat * rayDir4D;
	QVector3D rayDir(rayDir4D);
	rayDir.normalize();
	std::unordered_map<QVector2D, std::optional<float>> rayCastResult;
	for (auto& node : trigMap)
	{
		for (auto& trig : node.second)
		{
			rayCastResult[trig[0]] = {};
			rayCastResult[trig[1]] = {};
			rayCastResult[trig[2]] = {};

		}
	}
	auto* rayAccel = _rayAccels[_targetModel].get();
	_rayCaster->addAccelerator(rayAccel);
	for (auto& rayTarget : rayCastResult)
	{
		QVector4D origin4D(rayTarget.first.x(), rayTarget.first.y(), RAY_Z, 1);
		origin4D = transformMat * origin4D;
		QVector3D origin(origin4D);

		auto hits = _rayCaster->rayIntersectDirection(origin, rayDir);
		auto nearest = _rayCaster->getFirstFront(hits);
		if (nearest)
		{
			auto distLocal = nearest.value().distance / LABEL_SCALE;
			rayTarget.second = distLocal;
		}
	}




	//generate wall
	std::vector<QVector3D> path;
	path.emplace_back(0, 0, -depth);
	path.emplace_back(0, 0, depth);
	//DFS polytree

	std::deque<PolyNode*> s;
	s.push_back(&polytree);
	while (!s.empty())
	{
		auto curr = s.back();
		s.pop_back();
		if (!curr->Contour.empty())
		{
			std::vector<QVector3D> fContour;
			fContour.reserve(curr->Contour.size());
			std::transform(curr->Contour.begin(), curr->Contour.end(), std::back_inserter(fContour), [](const IntPoint& intPt)-> QVector3D {
				return QVector3D(toFloatPt(intPt));
			});
			auto jointDir = Hix::Features::Extrusion::interpolatedJointNormals(path);
			extrudeAlongPath(labelMesh, QVector3D(0, 0, 1), fContour, path, jointDir);
		}
		for (auto& each : curr->Childs)
		{
			s.push_back(each);
		}
	}



	//generate front & back mesh
	for (auto& node : trigMap)
	{
		for (auto& trig : node.second)
		{
			//std::array<float, 3> zProtrude;
			//std::array<float, 3> zDig;

			//for (auto i = 0; i < 3; ++i)
			//{
			//	auto& result = rayCastResult[trig[i]];
			//	if (result)
			//	{
			//		auto actualDist = result.value() - RAY_Z;
			//		zProtrude[i] = -actualDist + depth;
			//		zDig[i] = -actualDist - depth;
			//	}
			//	else
			//	{
			//		zProtrude[i] = depth;
			//		zDig[i] = -depth;
			//	}
			//}


			//protruded end
			labelMesh->addFace(
				QVector3D(trig[0].x(), trig[0].y(), depth),
				QVector3D(trig[1].x(), trig[1].y(), depth),
				QVector3D(trig[2].x(), trig[2].y(), depth)
			);
			//dug in end
			//need to raycast in order to dig deep enough
			labelMesh->addFace(
				QVector3D(trig[2].x(), trig[2].y(), -depth),
				QVector3D(trig[1].x(), trig[1].y(), -depth),
				QVector3D(trig[0].x(), trig[0].y(), -depth));
		}
	}


	auto& vertices = labelMesh->getVertices();
	for (auto itr = vertices.begin(); itr != vertices.end(); ++itr)
	{
		QVector2D xy(itr.localPosition());
		auto& result = rayCastResult[xy];
		if (result)
		{
			auto actualDist = result.value() - RAY_Z;
			itr->position.setZ(itr->position.z() - actualDist);
		}
		else
		{
		}

	}

	auto preview =  new GLModel(_targetModel, labelMesh, "label", 0);
	preview->transform().setMatrix(transformMat);
	return preview;
}



Hix::Features::LabellingMode::LabellingMode() : DialogedMode(LABEL_POPUP_URL)
{
	auto selected = Hix::Application::ApplicationManager::getInstance().partManager().selectedModels();
	if (selected.empty())
	{
		Hix::Application::ApplicationManager::getInstance().modalDialogManager().needToSelectModels();
		return;
	}
	std::unordered_set<const GLModel*> childs;

	for (auto model : selected)
	{
		model->getChildrenModels(childs);
		childs.insert(model);
	}

	_rayCaster.reset(new MTRayCaster());
	std::for_each(
		std::begin(childs),
		std::end(childs),
		[this](const GLModel* model) {  // copies are safer, and the resulting code will be as quick.
			_rayAccels[model] = std::make_unique<Hix::Engine3D::BVH>(*model, false);
		});	
	auto& co = controlOwner();
	co.getControl(_inputText, "labeltext");
	co.getControl(_fontStyle, "labelfont");
	co.getControl(_fontSize, "labelfontsize");
	co.getControl(_labelDepth, "labelDepth");
	co.getControl(_isEmboss, "labeltype");
}

Hix::Features::LabellingMode::~LabellingMode()
{
}
//IMPORTANT:
//this event is called per actual model selected, not the recursive listed parent.
//so this feature does not require recursion for GLModels
void Hix::Features::LabellingMode::faceSelected(GLModel* selected, const Hix::Engine3D::FaceConstItr& selectedFace, const Hix::Input::MouseEventData& mouse, const Qt3DRender::QRayCasterHit& hit)
{
	_targetModel = selected;
	updateLabelMesh(hit.localIntersection(), selectedFace);

}

void Hix::Features::LabellingMode::updateLabelMesh(const QVector3D& localIntersection, const Hix::Engine3D::FaceConstItr& face)
{

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
	newTransform.setScale(LABEL_SCALE);



	_previewModel.reset(generatePreviewModel(newTransform.matrix()));
	_previewModel->updateAABBScale(newTransform.scale3D());
	_matrix = newTransform.matrix();
	_scale = newTransform.scale3D();


}

void Hix::Features::LabellingMode::applyButtonClicked()
{
	if (!_previewModel)
	{
		return;
	}
	if (_isEmboss->isChecked())
	{
		Hix::Application::ApplicationManager::getInstance().taskManager().enqueTask(new Labelling(_targetModel, _previewModel.release()));

	}
	else
	{
		Hix::Application::ApplicationManager::getInstance().taskManager().enqueTask(new LabellingEngrave(_targetModel, _previewModel.release()));

	}
}



Hix::Features::Labelling::Labelling(GLModel* parentModel, GLModel* previewModel)
	:FlushSupport(parentModel),_targetModel(parentModel), _label(previewModel)
{
}

Hix::Features::Labelling::~Labelling()
{
}

void Hix::Features::Labelling::undoImpl()
{
	postUIthread([this]() {
		auto raw = std::get<GLModel*>(_label);
		raw->QNode::setParent((Qt3DCore::QNode*)nullptr);
		_label = std::unique_ptr<GLModel>(raw);
		});
}

void Hix::Features::Labelling::redoImpl()
{
	postUIthread([this]() {
		auto& owned = std::get<std::unique_ptr<GLModel>>(_label);
		if (!ApplicationManager::getInstance().partManager().isSelected(_targetModel))
			owned->setMaterialColor(Hix::Render::Colors::Default);
		owned->QNode::setParent(_targetModel);
		_label = owned.release();
		});

}

void Hix::Features::Labelling::runImpl()
{
	postUIthread([this]() {
		_targetModel->setMaterialColor(Hix::Render::Colors::Selected);
		_targetModel->setHitTestable(true);
		_targetModel->updateMesh(true);
		});
}

Hix::Features::LabellingEngrave::LabellingEngrave(GLModel* parentModel, GLModel* previewModel):FlushSupport(parentModel), _target(parentModel), _label(previewModel)
{
}

void Hix::Features::LabellingEngrave::cutCSG(GLModel* subject, const CorkTriMesh& subtract)
{
	//convert to cork-mesh, coordinate system same as before
	auto subjectCork = toCorkMesh(*subject->getMesh(), QMatrix4x4());
	CorkTriMesh output;
	computeDifference(subjectCork, subtract, &output);

	//convert the result back to hix mesh
	auto result = toHixMesh(output);
	//Hix::Features::repair(*result);
	//manual free cork memory, TODO RAII
	freeCorkTriMesh(&subjectCork);
	freeCorkTriMesh(&output);


	postUIthread([result, subject, this]() {
		_prevMesh.reset(subject->getMeshModd());
		subject->setMesh(result);
		_label.reset();
	});

}


void Hix::Features::LabellingEngrave::runImpl()
{
	//change label to cork mesh, change coordinate system to that of subjcect model
	auto subtractee = toCorkMesh(*_label->getMesh(), _label->transform().matrix());
	cutCSG(_target, subtractee);

	if (Hix::Features::isRepairNeeded(_target->getMesh()))
	{
		//Hix::Application::ApplicationManager::getInstance().setProgressText("Repairing mesh.");
		auto repair = new MeshRepair(_target);
		tryRunFeature(*repair);
	}
}

void Hix::Features::LabellingEngrave::undoImpl()
{
	postUIthread([this]() {
		auto tmp = _prevMesh.release();
		_prevMesh.reset(_target->getMeshModd());
		_target->setMesh(tmp);
		});
}

void Hix::Features::LabellingEngrave::redoImpl()
{
	postUIthread([this]() {
		auto tmp = _prevMesh.release();
		_prevMesh.reset(_target->getMeshModd());
		_target->setMesh(tmp);
		});
}

