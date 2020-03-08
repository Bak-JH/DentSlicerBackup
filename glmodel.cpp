#include <QRenderSettings>
#include "glmodel.h"
#include <QString>
#include <QtMath>
#include <cfloat>
#include <exception>


#include "feature/shelloffset.h"
//#include "feature/supportview.h"
#include "utils/utils.h"
#include <QtConcurrent/QtConcurrent>
#include <QFuture>
#include <QFileDialog>
#include <iostream>
#include <QDir>
#include <QMatrix3x3>
#include <Qt3DCore/qpropertyupdatedchange.h>

#include "feature/Extrude.h"
#include "feature/cut/modelcut.h"
#include "feature/cut/DrawingPlane.h"
#include "feature/label/labelling.h"
#include "feature/layFlat.h"
#include "feature/interfaces/SelectFaceMode.h"
#include "feature/move.h"
#include "application/ApplicationManager.h"

#define ATTRIBUTE_SIZE_INCREMENT 200
#if defined(_DEBUG) || defined(QT_DEBUG)
#define _STRICT_GLMODEL
#endif


using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Qt3DExtras;

using namespace Utils::Math;
using namespace Hix::Engine3D;
using namespace Hix::Input;
using namespace Hix::Render;
using namespace Hix::Application;

GLModel::GLModel(Qt3DCore::QEntity*parent, Mesh* loadMesh, QString fname, const Qt3DCore::QTransform* transform)
    : SceneEntityWithMaterial(parent)
    , _name(fname)
{
	QObject::connect(this, &QNode::enabledChanged, this, &GLModel::qnodeEnabledChanged);
	initHitTest();
    // set shader mode and color
	setMaterialMode(Hix::Render::ShaderMode::SingleColor);
	setMaterialColor(Hix::Render::Colors::Default);
	//this order is important since transform affect AABB calculation during setMesh
	if (transform)
	{
		_transform.setMatrix(transform->matrix());
	}
	setMesh(loadMesh);
	QObject::connect(this, SIGNAL(_updateModelMesh()), this, SLOT(updateModelMesh()));
}

//copy
GLModel::GLModel(const GLModel& o) : GLModel(o.parentEntity(), new Mesh(*o.getMesh()), o._name, &o._transform)
{
	//copy children models
	o.copyChildrenRecursive(this);
}

void GLModel::copyChildrenRecursive(GLModel* newParent) const
{
	for (auto child : childNodes())
	{
		auto model = dynamic_cast<GLModel*>(child);
		if (model)
		{
			auto copy = new GLModel(model);
			copy->setParent(newParent);
			callRecursive(model, &GLModel::copyChildrenRecursive, copy);
		}
	}
}

void GLModel::moveModel(const QVector3D& movement) {
	auto translation = _transform.translation() + movement;
	_transform.setTranslation(translation);
	updateAABBMove(movement);
}

void GLModel::rotateModel(const QQuaternion& rotation) {
	auto newRot = rotation * _transform.rotation();
	_transform.setRotation(newRot);
}


void GLModel::scaleModel(const QVector3D& scales) {
	auto newScales = _transform.scale3D() * scales; //this is NOT cross product
	_transform.setScale3D(newScales);
	updateAABBScale(scales);

}


void GLModel::updateAABBMove(const QVector3D& displacement)
{
	auto translation = _transform.translation() + displacement;
	_aabb.translate(displacement);
	callRecursive(this, &GLModel::updateAABBMove, displacement);
}
void GLModel::updateAABBScale(const QVector3D& scale)
{
	updateRecursiveAabb();
	//auto newScales = _transform.scale3D() * scale; //this is NOT cross product
	//_aabb.translate(-_transform.translation());
	//_aabb.scale(QVector3D(1.0f, 1.0f, 1.0f) / _transform.scale3D());
	//_aabb.scale(newScales);
	//_aabb.translate(_transform.translation());
	//callRecursive(this, &GLModel::updateAABBScale, scale);
}


void GLModel::flushTransform()
{
	_mesh->vertexApplyTransformation(_transform);
	_transform.setMatrix(QMatrix4x4());
	updateRecursiveAabb();
	updateMesh(_mesh);
}

void GLModel::moveDone()
{
	updatePrintable();
}


void GLModel::rotateDone()
{
	updateRecursiveAabb();
	setZToBed();
	updatePrintable();
}
void GLModel::scaleDone()
{

	setZToBed();
	updatePrintable();

}


void GLModel::setZToBed()
{
	auto listedModel = getRootModel();
	listedModel->moveModel(QVector3D(0, 0, -listedModel->recursiveAabb().zMin()));
}

QString GLModel::modelName() const
{
	return _name;
}

GLModel* GLModel::getRootModel()
{
	auto current = this;
	GLModel* back = current;
	while (current)
	{
		back = current;
		current = dynamic_cast<GLModel*>(current->parent());
	}
	return back;
}



void GLModel::changeColor(const QVector4D& color)
{
	setMaterialColor(color);
}

bool GLModel::isPrintable()const
{
	const auto& bedBound = Hix::Application::ApplicationManager::getInstance().settings().printerSetting.bedBound;
	return bedBound.contains(_aabb);
}

void GLModel::updatePrintable() {
	// is it inside the printing area or not?
	if(!isPrintable())
	{
		setMaterialColor(Hix::Render::Colors::OutOfBound);
	}
	else
	{
		if (ApplicationManager::getInstance().partManager().isSelected(this))
		{
			setMaterialColor(Hix::Render::Colors::Selected);
		}
		else
		{
			setMaterialColor(Hix::Render::Colors::Default);
		}
	}
}

GLModel::~GLModel()
{
}


void GLModel::getChildrenModels(std::unordered_set<const GLModel*>& results)const
{
	 
	for (auto child : childNodes())
	{
		auto model = dynamic_cast<GLModel*>(child);
		if (model)
		{
			results.insert(model);
		}
	}
	callRecursive(this, &GLModel::getChildrenModels, results);

}

void GLModel::initHitTest()
{
	addComponent(&_layer);
	_layer.setRecursive(false);

}

void GLModel::qnodeEnabledChanged(bool isEnabled)
{
	setHitTestable(isEnabled);
}

void GLModel::setHitTestable(bool isEnable)
{
	if (_hitEnabled != isEnable)
	{
		_hitEnabled = isEnable;
		if (_hitEnabled)
		{
			
			Hix::Application::ApplicationManager::getInstance().getRayCaster().addInputLayer(&_layer);
		}
		else
		{
			Hix::Application::ApplicationManager::getInstance().getRayCaster().removeInputLayer(&_layer);
		}
	}
	callRecursive(this, &GLModel::setHitTestable, isEnable);
}

int GLModel::ID() const
{
	int64_t longPtr = (int64_t)this;
	return longPtr;
}

void GLModel::clicked(MouseEventData& pick, const Qt3DRender::QRayCasterHit& hit)
{
	auto listed = getRootModel();
	auto& partManager = Hix::Application::ApplicationManager::getInstance().partManager();
	auto isSelected = partManager.isSelected(listed);
	if (!Hix::Application::ApplicationManager::getInstance().featureManager().isFeatureActive() || Hix::Application::ApplicationManager::getInstance().featureManager().isActive<Hix::Features::WidgetMode>())
	{
		if (pick.button == Qt::MouseButton::LeftButton)
		{
			//unselect previously selected parts if multi selection is not active.
			if (!isSelected && !partManager.isMultiSelect())
			{
				partManager.unselectAll();
			}
			partManager.setSelected(listed, !isSelected);
		}
		return;
	}

    //triangle index section
    if (hit.primitiveIndex() >= _mesh->getFaces().size() || hit.localIntersection() == QVector3D(0, 0, 0) || !isSelected)
    {
    }
	else
	{
		auto selectFaceFeature = dynamic_cast<Hix::Features::SelectFaceMode*>(Hix::Application::ApplicationManager::getInstance().featureManager().currentMode());
		if (selectFaceFeature)
		{
			auto selectedFace = _mesh->getFaces().cbegin() + hit.primitiveIndex();
			auto fn = selectedFace.worldFn();
			selectFaceFeature->faceSelected(this, selectedFace, pick, hit);
		}
	}
}
void GLModel::updateModelMesh() {
	updateMesh(_mesh);
}


bool GLModel::isDraggable(Hix::Input::MouseEventData& e,const Qt3DRender::QRayCasterHit&)
{
	auto listed = getRootModel();
	if (e.button == Qt3DInput::QMouseEvent::Buttons::LeftButton && ApplicationManager::getInstance().partManager().isSelected(listed)
		&& (!Hix::Application::ApplicationManager::getInstance().featureManager().isFeatureActive() ||
			Hix::Application::ApplicationManager::getInstance().featureManager().isActive<Hix::Features::MoveMode>()))
	{
		return true;
	}
	return false;
}

void GLModel::dragStarted(Hix::Input::MouseEventData& e, const Qt3DRender::QRayCasterHit& hit)
{
	//if(!Hix::Application::ApplicationManager::getInstance().featureManager().isActive<Hix::Features::MoveMode>())
	//	Hix::Application::ApplicationManager::getInstance().moveButton->setProperty("state", "active");
	auto& featureManager = Hix::Application::ApplicationManager::getInstance().featureManager();
	if (!featureManager.isActive<Hix::Features::MoveMode>())
	{
		featureManager.setMode(new Hix::Features::MoveMode());
	}
	dynamic_cast<Hix::Features::MoveMode*>(featureManager.currentMode())->featureStarted();
	auto listed = getRootModel();
	//if (Hix::Application::ApplicationManager::getInstance().supportRaftManager().supportActive())
	//{
	//	size_t prevCount = Hix::Application::ApplicationManager::getInstance().supportRaftManager().supportCount();
	//	Hix::Application::ApplicationManager::getInstance().supportRaftManager().clear(*listed);
	//	if (Hix::Application::ApplicationManager::getInstance().supportRaftManager().supportCount() != prevCount)
	//	{
	//		listed->setZToBed();
	//	}
	//}
	lastpoint = hit.localIntersection();
	prevPoint = (QVector2D)e.position;
	Hix::Application::ApplicationManager::getInstance().cursorManager().setCursor(CursorType::ClosedHand);

}

void GLModel::doDrag(Hix::Input::MouseEventData& v)
{
	QVector2D currentPoint = QVector2D(v.position.x(), v.position.y());

	QVector3D xAxis3D = QVector3D(1, 0, 0);
	QVector3D yAxis3D = QVector3D(0, 1, 0);
	QVector2D xAxis2D = (Hix::Application::ApplicationManager::getInstance().sceneManager().worldToScreen(lastpoint + xAxis3D) - Hix::Application::ApplicationManager::getInstance().sceneManager().worldToScreen(lastpoint));
	QVector2D yAxis2D = (Hix::Application::ApplicationManager::getInstance().sceneManager().worldToScreen(lastpoint + yAxis3D) - Hix::Application::ApplicationManager::getInstance().sceneManager().worldToScreen(lastpoint));
	QVector2D target = currentPoint - prevPoint;

	float b = (target.y() * xAxis2D.x() - target.x() * xAxis2D.y()) /
		(xAxis2D.x() * yAxis2D.y() - xAxis2D.y() * yAxis2D.x());
	float a = (target.x() - b * yAxis2D.x()) / xAxis2D.x();

	// move ax + by amount
	auto mode = dynamic_cast<Hix::Features::MoveMode*>(Hix::Application::ApplicationManager::getInstance().featureManager().currentMode());
	mode->modelMove(QVector3D(a, b, 0));
	prevPoint = currentPoint;
}

void GLModel::dragEnded(Hix::Input::MouseEventData&)
{
	dynamic_cast<Hix::Features::MoveMode*>(Hix::Application::ApplicationManager::getInstance().featureManager().currentMode())->featureEnded();
    //Hix::Application::ApplicationManager::getInstance().totalMoveDone();
}


/** HELPER functions **/

bool GLModel::EndsWith(const std::string& a, const std::string& b) {
    if (b.size() > a.size()) return false;
    return std::equal(a.begin() + a.size() - b.size(), a.end(), b.begin());
}


QVector3D GLModel::spreadPoint(QVector3D endPoint, QVector3D startPoint, int factor) {
	QVector3D standardVector = endPoint - startPoint;
	QVector3D finalVector = endPoint + standardVector * (factor - 1);
	return finalVector;
}
// for extension

void GLModel:: unselectMeshFaces(){
	selectedFaces.clear();
	callRecursive(this, &GLModel::unselectMeshFaces);
}

void GLModel::changeViewMode(int viewMode) {
 //   if( this->viewMode == viewMode ) {
 //       return;
 //   }

 //   this->viewMode = viewMode;
 //   qDebug() << "changeViewMode" << viewMode;
 //   QMetaObject::invokeMethod(Hix::Application::ApplicationManager::getInstance().boxUpperTab, "all_off");

 //   switch( viewMode ) {
 //   case VIEW_MODE_OBJECT:
 //       break;
 //   case VIEW_MODE_LAYER:
 //       break;
 //   }
	//updateShader(viewMode);


 //   emit _updateModelMesh();
}




bool GLModel::perPrimitiveColorActive() const
{
	return faceSelectionActive();
}
bool GLModel::faceSelectionActive() const
{
	return Hix::Application::ApplicationManager::getInstance().featureManager().isActive<Hix::Features::ExtendMode>() ||
		Hix::Application::ApplicationManager::getInstance().featureManager().isActive<Hix::Features::LayFlat>();
}

QVector4D GLModel::getPrimitiveColorCode(const Hix::Engine3D::Mesh* mesh, FaceConstItr itr)
{
	if (selectedFaces.find(itr) != selectedFaces.end())
	{
		return Hix::Render::Colors::SelectedFace;
	}
	else
	{
		return Hix::Render::Colors::Selected;
	}

}


QString GLModel::filenameToModelName(const std::string& s)
{
	char sep = '/';

	size_t i = s.rfind(sep, s.length());
	if (i != std::string::npos) {
		return QString::fromStdString(s.substr(i + 1, s.length() - i));
	}

	return QString::fromStdString(s);
}
