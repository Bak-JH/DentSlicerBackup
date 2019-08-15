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
	enum MeshType
	{
		ModelMesh = 0,
		Support,
		Raft
	};
    //probably interface this as well
	void clicked	(Hix::Input::MouseEventData&,const Qt3DRender::QRayCasterHit&);

	bool isDraggable(Hix::Input::MouseEventData& v,const Qt3DRender::QRayCasterHit&) override;
	void dragStarted(Hix::Input::MouseEventData&,const Qt3DRender::QRayCasterHit&) override;
	void doDrag(Hix::Input::MouseEventData& e)override;
	void dragEnded(Hix::Input::MouseEventData&) override;

	//size of QGeometry Attribute elements
	const static size_t POS_SIZE = 3; //x, y, z of position
	const static size_t NRM_SIZE = 3; //x, y, z of normal
	const static size_t COL_SIZE = 3; //x, y, z of normal
	const static size_t VTX_SIZE = (POS_SIZE + NRM_SIZE + COL_SIZE) * sizeof(float);

	const static size_t IDX_SIZE = 3; //3 indices to vertices
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
	Hix::Render::ModelMaterial m_meshMaterial;
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

	QTime getPrevTime();
	QTime getNextTime();


    void changeColor(const QVector3D& color);

    void setSupportAndRaft();


    // Model Mesh move
    void repairMesh();
    void moveModelMesh_direct(QVector3D direction, bool update = true);
    void moveModelMesh(QVector3D direction, bool update = true);
    // Model Mesh rotate
    void rotationDone();
    void rotateByNumber(QVector3D& rot_center, int X, int Y, int Z);
    void rotateModelMesh_direct(int Axis, float Angle, bool update = true);
    void rotateModelMesh(int Axis, float Angle, bool update = true);
    void rotateModelMesh_direct(QMatrix4x4 matrix, bool update = true);
    void rotateModelMesh(QMatrix4x4 matrix, bool update = true);
    // Model Mesh scale
    void scaleModelMesh(float scaleX, float scaleY, float scaleZ);
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

    const Hix::Engine3D::Mesh* getMesh();
    const Hix::Engine3D::Mesh* getSupport();
	const Mesh* getRaft();
	void setBoundingBoxVisible(bool isEnabled);
	const Qt3DCore::QTransform* getTransform() const;
	void setTranslation(const QVector3D& t);
    QVector3D getTranslation();
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

	bool perPrimitiveColorActive()const;
	bool faceHighlightActive()const;
	bool raftSupportGenerated()const;
private:
	bool _raftSupportGenerated = false;
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
	//separates SSBO buffer for Per-primitive colors
	//Qt3DRender::QBuffer primitiveColorBuffer;
    QAttribute positionAttribute;
    QAttribute normalAttribute;
	QAttribute colorAttribute;

    QAttribute indexAttribute;
	//enum Hix::Render::MeshColorCodes is 32bits long due to the way c++ handles enums, so we force 8bit uint here.
	std::vector<uint8_t> perPrimitiveColors;
	//QVariantList _primitiveColorCodes;
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
	//unsigned int getPrimitiveColorCode(const Hix::Engine3D::Mesh* mesh, size_t faceIdx);
	QVector3D getPrimitiveColorCode(const Hix::Engine3D::Mesh* mesh, FaceConstItr faceItr);

	void setMesh(Hix::Engine3D::Mesh* mesh);
	void updateMesh(Hix::Engine3D::Mesh* mesh);
	void appendMesh(Hix::Engine3D::Mesh* mesh);
	void appendMeshVertex(const Hix::Engine3D::Mesh* mesh,
		Hix::Engine3D::FaceConstItr begin, Hix::Engine3D::FaceConstItr end);

	void appendMeshVertexSingleColor(const Hix::Engine3D::Mesh* mesh,
		Hix::Engine3D::FaceConstItr begin, Hix::Engine3D::FaceConstItr end);
	void appendMeshVertexPerPrimitive(const Hix::Engine3D::Mesh* mesh,
		Hix::Engine3D::FaceConstItr begin, Hix::Engine3D::FaceConstItr end);

	void appendIndexArray(const Hix::Engine3D::Mesh* mesh,
		Hix::Engine3D::FaceConstItr begin, Hix::Engine3D::FaceConstItr end);

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
    void updateModelMesh();

    // Generate support mesh
    void generateSupport();

    //TODO: get rid of this
    friend class featureThread;
    friend class STLexporter;
};



#endif // GLMODEL_H
