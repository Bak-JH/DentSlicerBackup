#ifndef GLMODEL_H
#define GLMODEL_H

#include "render/SceneEntityWithMaterial.h"
#include "fileloader.h"
#include "slice/slicingengine.h"
#include "feature/modelcut.h"
#include "feature/labellingtextpreview.h"
#include "feature/autoorientation.h"
#include "feature/meshrepair.h"
#include "feature/autoarrange.h"
#include "feature/extension.h"
#include "feature/hollowshell.h"
#include "input/raycastcontroller.h"
#include "input/Draggable.h"
#include "input/Clickable.h"
#include "support/SupportRaftManager.h"

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


class GLModel : public Hix::Render::SceneEntityWithMaterial, public Hix::Input::Draggable, public Hix::Input::Clickable
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
    //QVector3D m_translation;

    // feature hollowshell
    float hollowShellRadius = 0;

    // face selection enabled
    std::unordered_set<FaceConstItr> selectedFaces;

    // feature offset
    double shellOffsetFactor;

    std::vector<QVector3D> cuttingPoints;
    std::vector<QEntity*> cuttingContourCylinders;
    Plane cuttingPlane;

    // used for layer view
    Qt3DExtras:: QPlaneMesh* layerViewPlane = nullptr;
    Qt3DCore::QEntity* layerViewPlaneEntity = nullptr;
    Qt3DCore::QTransform *layerViewPlaneTransform = nullptr;
    Qt3DRender::QTextureLoader* layerViewPlaneTextureLoader = nullptr;
    Qt3DExtras::QTextureMaterial* layerViewPlaneMaterial = nullptr;
    //Qt3DExtras::QPhongAlphaMaterial *layerViewPlaneMaterial = nullptr;


    Qt3DExtras::QPlaneMesh* clipPlane[2];
    Qt3DCore::QEntity* planeEntity[2];
    Qt3DCore::QTransform *planeTransform[2];
    Qt3DExtras::QPhongAlphaMaterial *planeMaterial = nullptr;
    QObjectPicker* planeObjectPicker[2];

    std::vector<Qt3DExtras::QSphereMesh*> sphereMesh;
    std::vector<Qt3DCore::QEntity*> sphereEntity;
    std::vector<Qt3DCore::QTransform*> sphereTransform;
    std::vector<Qt3DRender::QObjectPicker*> sphereObjectPicker;
    std::vector<QPhongMaterial*> sphereMaterial;

    void removeModelPartList();
    LabellingTextPreview* labellingTextPreview = nullptr;

    void copyModelAttributeFrom(GLModel* from);


    void changeColor(const QVector3D& color);

    void setSupportAndRaft();


    void repairMesh();

    // Model Cut
    void addCuttingPoint(QVector3D v);
    void removeCuttingPoint(int idx);
    void removeCuttingPoints();
    void drawLine(QVector3D endpoint);

    void checkPrintingArea();
    bool EndsWith(const std::string& a, const std::string& b);
    bool modelSelectChangable();
    QString getFileName(const std::string& s);
    static QVector3D spreadPoint(QVector3D endpoint,QVector3D startpoint,int factor);
    void changeViewMode(int viewMode);
	void updateShader(int viewMode);
    void inactivateFeatures();


    const int ID; //for use in Part List
    QString filename;
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
	bool manualSupportActive = false;
	bool layerViewActive = false;
	bool supportViewActive = false;
	bool scaleActive = false;
	bool isMoved = false;

	bool perPrimitiveColorActive()const;
	bool faceHighlightActive()const;
	bool raftSupportGenerated()const;



	//TODO: remove these
	// Model Mesh move, rotate, scale
	void moveModelMesh(QVector3D direction, bool update = true);
	void rotationDone();
	void rotateByNumber(QVector3D& rot_center, int X, int Y, int Z);
	void rotateModelMesh(int Axis, float Angle, bool update = true);
	void rotateModelMesh(QMatrix4x4 matrix, bool update = true);
	void scaleModelMesh(float scaleX, float scaleY, float scaleZ);


private:
	void adjustZHeight(int viewMode);
	QVector3D getPrimitiveColorCode(const Hix::Engine3D::Mesh* mesh, FaceConstItr faceItr)override;

    //Order is important! Look at the initializer list in constructor
	Hix::Support::SupportRaftManager _supportRaftManager;
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
    void layFlatUnSelect();
    void extensionSelect();
    void extensionUnSelect();

public slots:
	void updateModelMesh();

    // object picker parts
    void mouseEnteredFreeCutSphere();
    void mouseExitedFreeCutSphere();
    void mouseClickedFreeCutSphere(Qt3DRender::QPickEvent*);
    void mouseClickedFreeCut(Qt3DRender::QPickEvent*);
    void mouseClickedLayflat(MeshFace shadow_meshface);


    // Scale
    void openScale();
    void closeScale();

    // Lay Flat
    void openLayflat();
    void closeLayflat();
    void generateLayFlat();

    // Model Cut
    void removeCuttingContour();
    void generateCuttingContour(std::vector<QVector3D> cuttingContour);
    void regenerateCuttingPoint(std::vector<QVector3D> cuttingContour);
    void generateClickablePlane();
    void generatePlane();
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
    void getTextChanged(QString text, int contentWidth);
    void openLabelling();
    void closeLabelling();
    void stateChangeLabelling();
    void getFontNameChanged(QString fontName);
    void getFontBoldChanged(bool isBold);
    void getFontSizeChanged(int fontSize);
    void applyLabelInfo(QString text, int contentWidth, QString fontName, bool isBold, int fontSize);
    void generateText3DMesh();

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

    // Manual Support
    void openManualSupport();
    void closeManualSupport();
    void generateManualSupport();

    // Model Mesh info update


    //TODO: get rid of this
    friend class featureThread;
    friend class STLexporter;
};



#endif // GLMODEL_H
