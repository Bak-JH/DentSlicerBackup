#ifndef GLMODEL_H
#define GLMODEL_H

#include "render/SceneEntityWithMaterial.h"
#include "fileloader.h"
#include "slice/slicingengine.h"
#include "feature/modelcut.h"
#include "feature/autoorientation.h"
#include "feature/meshrepair.h"
#include "feature/autoarrange.h"
#include "feature/extension.h"
#include "feature/hollowshell.h"
#include "input/raycastcontroller.h"
#include "input/Draggable.h"
#include "input/Clickable.h"
#include "input/HitTestAble.h"

#include "render/ModelMaterial.h"
#define MAX_BUF_LEN 2000000

using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Qt3DExtras;


/* feature thread */
#define ftrEmpty -1 //TODO: remove this swtich craziness.
#define ftrOpen 1
#define ftrSave 2
#define ftrExport 3
#define ftrMove 4
#define ftrRotate 5
#define ftrLayFlat 6
#define ftrArrange 7
#define ftrOrient 8
#define ftrScale 9
#define ftrRepair 10
#define ftrCut 11
#define ftrShellOffset 12
#define ftrExtend 13
#define ftrManualSupport 14
#define ftrLabel 15
#define ftrSupportViewMode 16
#define ftrLayerViewMode 17
#define ftrDelete 18
#define ftrTempExport 19
#define ftrSupportDisappear 20


class GLModel;
class OverhangPoint;

namespace Hix
{
	class LabelModel;
}

namespace Hix
{
	namespace Features
	{
		namespace Cut
		{
			class DrawingPlane;
		}
	}
}


class GLModel : public Hix::Render::SceneEntityWithMaterial, public Hix::Input::Draggable, public Hix::Input::Clickable, public Hix::Input::HitTestAble
{
    Q_OBJECT
public:

    //probably interface this as well
	void clicked	(Hix::Input::MouseEventData&,const Qt3DRender::QRayCasterHit&) override;
	bool isDraggable(Hix::Input::MouseEventData& v,const Qt3DRender::QRayCasterHit&) override;
	void dragStarted(Hix::Input::MouseEventData&,const Qt3DRender::QRayCasterHit&) override;
	void doDrag(Hix::Input::MouseEventData& e)override;
	void dragEnded(Hix::Input::MouseEventData&) override;

    // load teeth model default
    GLModel(QObject* mainWindow=nullptr, QEntity* parent=nullptr, Hix::Engine3D::Mesh* loadMesh=nullptr, QString fname="", int id = 0); // main constructor for mainmesh and shadowmesh
    virtual ~GLModel();

    //TODO: Turn these into privates as well
    GLModel *leftModel = nullptr;
    GLModel *rightModel = nullptr;
    GLModel *twinModel = nullptr; // saves cut right for left, left for right models


    bool appropriately_rotated=false;

    // feature hollowshell
    float hollowShellRadius = 0;
    // face selection enabled
    std::unordered_set<FaceConstItr> selectedFaces;

    // feature offset
    double shellOffsetFactor;


    // used for layer view
    Qt3DExtras:: QPlaneMesh* layerViewPlane = nullptr;
    Qt3DCore::QEntity* layerViewPlaneEntity = nullptr;
    Qt3DCore::QTransform *layerViewPlaneTransform = nullptr;
    Qt3DRender::QTextureLoader* layerViewPlaneTextureLoader = nullptr;
    Qt3DExtras::QTextureMaterial* layerViewPlaneMaterial = nullptr;
    //Qt3DExtras::QPhongAlphaMaterial *layerViewPlaneMaterial = nullptr;




    std::vector<Qt3DExtras::QSphereMesh*> sphereMesh;
    std::vector<Qt3DCore::QEntity*> sphereEntity;
    std::vector<Qt3DCore::QTransform*> sphereTransform;
    std::vector<Qt3DRender::QObjectPicker*> sphereObjectPicker;
    std::vector<QPhongMaterial*> sphereMaterial;

    void removeModelPartList();
	Hix::LabelModel* textPreview = nullptr;

    void copyModelAttributeFrom(GLModel* from);


    void changeColor(const QVector3D& color);



    void repairMesh();

    // Model Cut
    void addCuttingPoint(QVector3D v);
    void removeCuttingPoint(int idx);
    void removeCuttingPoints();
    void drawLine(QVector3D endpoint);
	bool isPrintable()const;
    void updatePrintable();
    bool EndsWith(const std::string& a, const std::string& b);
    QString getFileName(const std::string& s);
    static QVector3D spreadPoint(QVector3D endpoint,QVector3D startpoint,int factor);
    void changeViewMode(int viewMode);
	void updateShader(int viewMode);
    void inactivateFeatures();


    const int ID; //for use in Part List
    QObject* mainWindow;

    // implement lock as bool variable
    bool updateLock;

	void setBoundingBoxVisible(bool isEnabled);

	
	bool labellingActive = false;
	bool extensionActive = false;
	bool cutActive = false;
	bool hollowShellActive = false;
	bool shellOffsetActive = false;
	bool layflatActive = false;
	bool layerViewActive = false;



	bool isMoved = false;

	// useless funcitons. why did you use them?
	bool perPrimitiveColorActive()const;
	bool faceSelectionActive()const;


	//TODO: remove these
	// Model Mesh move, rotate, scale
	void moveModel(const QVector3D& displacement);
	void rotateModel(const QQuaternion& rotation);
	void scaleModel(const QVector3D& scale);
	void moveDone();
	void rotateDone();
	void scaleDone();
	void setZToBed();
	QString filename()const;

	void setHitTestable(bool isEnable);

protected:
	void initHitTest()override;

private:
	QString _filename;

	//cutting
	std::unique_ptr<Hix::Features::Cut::DrawingPlane> _cuttingPlane;


	QVector3D getPrimitiveColorCode(const Hix::Engine3D::Mesh* mesh, FaceConstItr faceItr)override;

    //Order is important! Look at the initializer list in constructor
    QVector3D lastpoint;
    QVector2D prevPoint;


	void removeLayerViewComponents();

    int cutMode = 1;
    int cutFillMode = 1;
    bool isFlatcutEdge = false;
    int viewMode = -1;

signals:
	void _updateModelMesh();
    void modelSelected(int);
    void resetLayflat();
    void bisectDone(Mesh*, Mesh*); //lmesh, rmesh
    void layFlatSelect();
	void manualSupportSelect();
	void manualSupportUnSelect();

    void layFlatUnSelect();
    void extensionSelect();
    void extensionUnSelect();

public slots:
	void updateModelMesh();


    // Scale

    // Lay Flat
    void openLayflat();
    void closeLayflat();
    void generateLayFlat();

    // Model Cut

    void generatePlane(int type);
    void removePlane();
    void modelCut();
    void cutModeSelected(int type);
    void cutFillModeSelected(int type);
    void getSliderSignal(double value);
    void getLayerViewSliderSignal(int value);
	void generateRLModel(Mesh* lmesh, Mesh* rmesh);
    void openCut();
    void closeCut();

    // Hollow Shell
    void indentHollowShell(double radius);
    void openHollowShell();
    void closeHollowShell();

    // Labelling
    void getTextChanged(QString text);
    void openLabelling();
    void closeLabelling();
    void stateChangeLabelling();
    void getFontNameChanged(QString fontName);
    void getFontBoldChanged(bool isBold);
    void getFontSizeChanged(int fontSize);
    void updateLabelPreview(QString text, QString fontName, bool isBold, int fontSize);
    void generateLabelMesh();

    // Extension
    void openExtension();
    void closeExtension();
    void unselectMeshFaces();
    void selectMeshFaces();
    void generateExtensionFaces(double distance);

    // ShellOffset
    void openShellOffset();
    void closeShellOffset();
    void generateShellOffset(double factor);

    // Model Mesh info update


    //TODO: get rid of this
    friend class featureThread;
    friend class STLexporter;
};



#endif // GLMODEL_H
