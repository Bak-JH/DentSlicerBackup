#pragma once

#include "render/SceneEntityWithMaterial.h"
#include "fileloader.h"
#include "slice/slicingengine.h"
#include "feature/autoorientation.h"
#include "feature/extension.h"
#include "feature/hollowshell.h"
#include "input/raycastcontroller.h"
#include "input/Draggable.h"
#include "input/Clickable.h"
#include "input/HitTestAble.h"

#include "render/ModelMaterial.h"
#define MAX_BUF_LEN 2000000


class GLModel;
class OverhangPoint;


class GLModel : public Hix::Render::SceneEntityWithMaterial, public Hix::Input::Draggable, public Hix::Input::Clickable, public Hix::Input::HitTestAble
{
    Q_OBJECT
public:
	static QString filenameToModelName(const std::string& s);
    //probably interface this as well
	void clicked	(Hix::Input::MouseEventData&,const Qt3DRender::QRayCasterHit&) override;
	bool isDraggable(Hix::Input::MouseEventData& v,const Qt3DRender::QRayCasterHit&) override;
	void dragStarted(Hix::Input::MouseEventData&,const Qt3DRender::QRayCasterHit&) override;
	void doDrag(Hix::Input::MouseEventData& e)override;
	void dragEnded(Hix::Input::MouseEventData&) override;

	void modelSelectionClick();
    // load teeth model default
    GLModel(Qt3DCore::QEntity* parent=nullptr, Hix::Engine3D::Mesh* loadMesh=nullptr, QString fname="", const Qt3DCore::QTransform* transform = nullptr);
	GLModel(const GLModel& o);
	virtual ~GLModel();

	void copyChildrenRecursive(GLModel* newParent)const;
	void getChildrenModels(std::unordered_set<const GLModel*>& results)const;


    // face selection enabled
    std::unordered_set<FaceConstItr> selectedFaces;

    void changeColor(const QVector4D& color);
	bool isPrintable()const;
    void updatePrintable();
    bool EndsWith(const std::string& a, const std::string& b);
    static QVector3D spreadPoint(QVector3D endpoint,QVector3D startpoint,int factor);


	//void setBoundingBoxVisible(bool isEnabled);
	void moveModel(const QVector3D& movement);
	void rotateModel(const QQuaternion& rotation);
	void scaleModel(const QVector3D& scale);
	void updateAABBMove(const QVector3D& translate);
	void updateAABBScale(const QVector3D& scale);

	void flushTransform();
	void moveDone();
	void rotateDone();
	void scaleDone();
	float setZToBed();
	void setHitTestable(bool isEnable);
	//tmp
	int ID()const;
	QString modelName()const;
	GLModel* getRootModel();
	const GLModel* getRootModel()const;

	void unselectMeshFaces();

protected:
	void initHitTest()override;
	void qnodeEnabledChanged(bool isEnabled);

private:
	QString _name;
    QVector3D lastpoint;
    QVector2D prevPoint;
	QVector4D getPrimitiveColorCode(const Hix::Engine3D::Mesh* mesh, FaceConstItr faceItr)override;


};
