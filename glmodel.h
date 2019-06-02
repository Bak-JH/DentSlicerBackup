#ifndef GLMODEL_H
#define GLMODEL_H

#include <QObject>
#include <QApplication>
#include <Qt3DCore>
#include <Qt3DRender>
#include <Qt3DExtras>
#include <Qt3DInput>
#include <QBuffer>
#include <QObjectPicker>
#include <QCursor>
#include <iostream>
#include <memory>
#include "DentEngine/src/mesh.h"
#include "fileloader.h"
#include "slicingengine.h"
#include "feature/modelcut.h"
#include "feature/labellingtextpreview.h"
#include "feature/autoorientation.h"
#include "feature/meshrepair.h"
#include "feature/autoarrange.h"
#include "feature/extension.h"
#include "feature/hollowshell.h"
#include "DentEngine/src/configuration.h"
#include "input/raycastcontroller.h"
#include "input/Draggable.h"
#define MAX_BUF_LEN 2000000

using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Qt3DExtras;


/* feature thread */
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
class featureThread: public QThread
{
    Q_OBJECT
public:
    featureThread(GLModel* glmodel, int type);
    GLModel* m_glmodel;
    int progress;
    int optype; // defines typeofoperation
    QVariant data;
    double arg1;
    double arg2;
    double arg3;
    autoorientation* ot;
    modelcut* ct;
    autoarrange* ar;
    STLexporter* ste;
    SlicingEngine* se;

signals:
    void loadPopup(QVariant value);

public slots:
    void setTypeAndRun(int type);
    void setTypeAndRun(int type, double arg1, double arg2, double arg3);
    void setTypeAndRun(int type, QVariant data);
    void setTypeAndStart(int type);
private:
    void run() Q_DECL_OVERRIDE;
};

class arrangeSignalSender: public QObject
{
    Q_OBJECT
public:
    arrangeSignalSender();
signals:
    void runArrange();
};



class GLModel : public QEntity, public Hix::Input::Draggable
{
    Q_OBJECT
public:

    //probably interface this as well
	void clicked	(Hix::Input::MouseEventData&,const Qt3DRender::QRayCasterHit&);

	bool isDraggable(Hix::Input::MouseEventData& v,const Qt3DRender::QRayCasterHit&) override;
	void dragStarted(Hix::Input::MouseEventData&,const Qt3DRender::QRayCasterHit&) override;
	void doDrag(Hix::Input::MouseEventData& e)override;
	void dragEnded(Hix::Input::MouseEventData&) override;

	//size of QGeometry Attribute elements
	const static size_t POS_SIZE = 3; //x, y, z of position
	const static size_t NRM_SIZE = 3; //x, y, z of normal
	const static size_t COL_SIZE = 3; //r, g, b for color
	const static size_t VTX_SIZE = (POS_SIZE + NRM_SIZE + COL_SIZE) * sizeof(float);

	const static size_t IDX_SIZE = 3; //3 index to vertices
	const static size_t UINT_SIZE = sizeof(uint); //needs to be large enough to accomodate all range of vertex index
	const static size_t FACE_SIZE = IDX_SIZE * UINT_SIZE;
    // load teeth model default
    GLModel(QObject* mainWindow=nullptr, QNode* parent=nullptr, Hix::Engine3D::Mesh* loadMesh=nullptr, QString fname="", int id = 0); // main constructor for mainmesh and shadowmesh
    virtual ~GLModel();

    //TODO: Turn these into privates as well
    GLModel *leftModel = nullptr;
    GLModel *rightModel = nullptr;
    GLModel *twinModel = nullptr; // saves cut right for left, left for right models


    bool appropriately_rotated=false;
	QPhongMaterial* m_meshMaterial;
    QPerVertexColorMaterial *m_meshVertexMaterial;
    QMaterial *m_layerMaterial;
    QParameter *m_layerMaterialHeight;
    QParameter *m_layerMaterialRaftHeight;
    //QVector3D m_translation;

    // feature hollowshell
    float hollowShellRadius = 0;

    // feature extension
    std::vector<FaceConstItr> extendFaces;

    // feature offset
    double shellOffsetFactor;

    std::vector<QVector3D> cuttingPoints;
    std::vector<QEntity*> cuttingContourCylinders;
    Plane cuttingPlane;

    // used for layer view
    Qt3DExtras:: QPlaneMesh* layerViewPlane[1] = {nullptr};
    Qt3DCore::QEntity* layerViewPlaneEntity[1] = {nullptr};
    Qt3DCore::QTransform *layerViewPlaneTransform[1] = {nullptr};
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

	QTime getPrevTime();
	QTime getNextTime();

    // changeColor
    void changecolor(int mode); //0 default, 1 selected, 2 outofarea

    void setSupportAndRaft();


    // Model Mesh move
    void repairMesh();
    void moveModelMesh(QVector3D direction, bool update = true);
    // Model Mesh rotate
    void rotationDone();
    void rotateByNumber(QVector3D& rot_center, int X, int Y, int Z);
    void rotateModelMesh(int Axis, float Angle, bool update = true);
    void rotateModelMesh(QMatrix4x4 matrix, bool update = true);
    // Model Mesh scale
    void scaleModelMesh(float scaleX, float scaleY, float scaleZ);
    // Model Cut
    void addCuttingPoint(QVector3D v);
    void removeCuttingPoint(int idx);
    void removeCuttingPoints();
    void drawLine(QVector3D endpoint);
	void bisectModel(Plane plane, Hix::Engine3D::Mesh& lmesh, Hix::Engine3D::Mesh& rmesh);

    void checkPrintingArea();
    bool EndsWith(const std::string& a, const std::string& b);
    bool modelSelectChangable();
    QString getFileName(const std::string& s);
    static QVector3D spreadPoint(QVector3D endpoint,QVector3D startpoint,int factor);
    void changeViewMode(int viewMode);
    void inactivateFeatures();

    // support
    Slicer* slicer;

    featureThread* ft;
    //arrangeSignalSender* arsignal; //unused, signal from qml goes right into QmlManager.runArrange
    QFutureWatcher<Slicer*> futureWatcher; // future watcher for feature thread results returned

    const int ID; //for use in Part List
    QString filename;
    QObject* mainWindow;
    QString slicingInfo;

    // implement lock as bool variable
    bool updateLock;

    const Hix::Engine3D::Mesh* getMesh();
    const Hix::Engine3D::Mesh* getSupport();
	const Mesh* getRaft();
	void setBoundingBoxVisible(bool isEnabled);
	const Qt3DCore::QTransform* getTransform() const;
	void setTranslation(const QVector3D& t);
	void setMatrix(const QMatrix4x4& matrix);
	Qt3DRender::QLayer* getLayer();
	/***************Ray casting and hit test***************/
	void setHitTestable(bool isEnabled);
	bool isHitTestable();

	//
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

private:

	//consts
	//for QAttributes
	Qt3DCore::QTransform m_transform;
	bool _hitEnabled = false;

    //Order is important! Look at the initializer list in constructor
	const Hix::Engine3D::Mesh* _currentVisibleMesh;
    QGeometryRenderer m_geometryRenderer;
    QGeometry m_geometry;

	//3 vectors per data, each for position, normal, color
    Qt3DRender::QBuffer vertexBuffer;
	//defines mesh faces by 3 indices to the vertexArray
    Qt3DRender::QBuffer indexBuffer;

    QAttribute positionAttribute;
    QAttribute normalAttribute;
    QAttribute colorAttribute;
    QAttribute indexAttribute;

    int colorMode;
    int v_cnt;
    int f_cnt;
    QNode* m_parent;
    QVector3D lastpoint;
    QVector2D prevPoint;
    void clearMem();
	//update faces given indicies, if index >= indexUppderLimit, it's ignored
	void updateFaces(const std::unordered_set<size_t>& faceIndicies, const Hix::Engine3D::Mesh& mesh);
	void updateVertices(const std::unordered_set<size_t>& vtxIndicies, const Hix::Engine3D::Mesh& mesh);

    void onTimerUpdate();
    void removeLayerViewComponents();
    void generateLayerViewMaterial();
	void setMesh(Hix::Engine3D::Mesh* mesh);
	void updateMesh(Hix::Engine3D::Mesh* mesh);
	void appendMesh(Hix::Engine3D::Mesh* mesh);
	size_t appendMeshVertex(const Hix::Engine3D::Mesh* mesh,
		Hix::Engine3D::VertexConstItr begin, Hix::Engine3D::VertexConstItr end);
	void appendMeshFace(const Hix::Engine3D::Mesh* mesh,
		Hix::Engine3D::FaceConstItr begin, Hix::Engine3D::FaceConstItr end, size_t prevMaxIndex);

    int cutMode = 1;
    int cutFillMode = 1;



    bool isFlatcutEdge = false;

    int viewMode = -1;

    // Core mesh structures
	Hix::Engine3D::Mesh* _mesh;

	Hix::Engine3D::Mesh* supportMesh = nullptr;
	Hix::Engine3D::Mesh* raftMesh = nullptr;

    // layer view
	Hix::Engine3D::Mesh* layerMesh;
	Hix::Engine3D::Mesh* layerSupportMesh;
	Hix::Engine3D::Mesh* layerRaftMesh;
	Hix::Engine3D::Mesh* layerInfillMesh;




	/***************Ray casting and hit test***************/
	Qt3DRender::QLayer _layer;
	bool _targetSelected = false;
	FaceConstItr targetMeshFace; // used for object selection (specific area, like extension or labelling)

signals:

    void modelSelected(int);
    void resetLayflat();
    void bisectDone(Mesh*, Mesh*); //lmesh, rmesh
    void _generateSupport();
    void _updateModelMesh();
    void layFlatSelect();
    void layFlatUnSelect();
    void extensionSelect();
    void extensionUnSelect();

public slots:
    // Model Undo & Redo
    void saveUndoState();
    void saveUndoState_internal();
    void loadUndoState();
    void loadRedoState();

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
    void getLayerViewSliderSignal(double value);
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
    void colorExtensionFaces();
    void uncolorExtensionFaces();
    void generateColorAttributes();
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
    void updateModelMesh();

    // Generate support mesh
    void generateSupport();
    void slicingDone();

    //TODO: get rid of this
    friend class featureThread;
    friend class STLexporter;
};



#endif // GLMODEL_H
