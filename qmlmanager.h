#ifndef QMLMANAGER_H
#define QMLMANAGER_H
#include <QObject>
#include <QDebug>
#include <QString>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlEngine>
#include <QMouseEvent>
#include <QCoreApplication>
#include <QEvent>
#include <QPointF>
#include <QCursor>
#include <QQmlProperty>
#include "feature/autoarrange.h"
#include "feature/shelloffset.h"
#include "glmodel.h"
#include "QtConcurrent/QtConcurrentRun"
#include "QFuture"
#include "utils/httpreq.h"
#include <QKeyboardHandler>
#include "input/raycastcontroller.h"
#include "ui/RotateXYZWidget.h"
#include "ui/MoveXYZWidget.h"
#define VIEW_MODE_OBJECT 0
#define VIEW_MODE_SUPPORT 1
#define VIEW_MODE_LAYER 2

#define LAYER_INFILL 0x01
#define LAYER_SUPPORTERS 0x02
#define LAYER_RAFT 0x04

class QQuickItem;
class QmlManager : public QObject
{
    Q_OBJECT
public:
    explicit QmlManager(QObject *parent = nullptr);
    QQmlApplicationEngine* engine;

    QString version = "0.0.1";

    // UI components
	Qt3DRender::QCamera* _camera;
    QObject* mainWindow;
    QObject* loginWindow;
    QObject* loginButton;
    QObject* boxUpperTab;
    QObject* boxLeftTab;
	QQuickItem* scene3d;
    QEntity* models;
    Qt3DCore::QTransform* systemTransform;
	QEntity* mv;
	Qt3DCore::QEntity* boundedBox;
    Qt3DCore::QEntity *mttab;
    QObject* undoRedoButton;
    QObject* slicingData;
    Qt3DInput::QKeyboardHandler* keyboardHandler;

    // model rotate components
    QObject *rotatePopup;

    // model move components
    QObject *moveButton;
    QObject *movePopup;
    Qt3DCore::QEntity *managerModel;

    // selection popup
    QObject* yesno_popup;
    QObject* result_popup;

    // model layflat components;
    QObject *layflatPopup;
    QObject *layflatButton;

    QObject* partList;

    // model cut components
    QObject *cutPopup;
    QObject *curveButton;
    QObject *flatButton;
    QObject *cutSlider;

    // hollow shell components
    QObject *hollowShellPopup;
    QObject *hollowShellSlider;
    Qt3DExtras::QSphereMesh* hollowShellSphereMesh;
    Qt3DCore::QEntity* hollowShellSphereEntity;
    Qt3DCore::QTransform* hollowShellSphereTransform;
    QPhongMaterial* hollowShellSphereMaterial;

    // labelling components
    QObject *text3DInput;
    QObject *labelPopup;
    QObject *labelFontBox;
    QObject *labelFontBoldBox;
    QObject *labelFontSizeBox;

    // orientation components
    QObject* orientPopup;
    QObject* progress_popup;
    QObject* orientButton;

    // scale components
    QObject* scalePopup;

    // extension components
    QObject* extensionPopup;
    QObject* extensionButton;

    // shell offset components
    QObject* shelloffsetPopup;

    // manual support components
    QObject* manualSupportPopup;

    // auto repair components
    QObject* repairPopup;
    QObject* repairButton;

    // auto arrange components
    //QObject* arrangePopup;

    // save components
    QObject* saveButton;
    //QObject* savePopup;

    // export components
    //QObject* exportButton;
    QObject* exportOKButton;

    // view mode buttons
    QObject* leftTabViewMode;
    QObject* viewObjectButton;
    QObject* viewSupportButton;
    QObject* viewLayerButton;

    QObject* layerViewPopup;
    QObject* layerInfillButton;
    QObject* layerSupportersButton;
    QObject* layerRaftButton;
    QObject* layerViewSlider;

    std::map<int, GLModel> glmodels;

    std::vector<Mesh*> copyMeshes;
    std::vector<QString> copyMeshNames;


	//!
    int rotateSnapAngle = 0;
    int rotateSnapStartAngle = 0;
    int rotateSnapQuotient = 0;
    //bool moveActive = false;
    bool saveActive = false;
    bool rotateActive = false;
    bool orientationActive = false;
    bool freecutActive = false;

	const Hix::Input::RayCastController& getRayCaster();
    QString groupFunctionState;
    int groupFunctionIndex;
    float progress = 0;
    void showRotateSphere();
    void showMoveArrow();
    void hideRotateSphere();
    void hideMoveArrow();
    void initializeUI(QQmlApplicationEngine *e);
    void openModelFile_internal(QString filename);
    void openArrange();
    void runArrange_internal();
    void disconnectHandlers(GLModel* glmodel);
	void disconnectShadow(GLModel* glmodel);
    void connectHandlers(GLModel* glmodel);
    void addPart(QString fileName, int ID);
    void deletePartListItem(int ID);
    void openProgressPopUp();
    void openYesNoPopUp(bool selectedList_vis, std::string inputText_h, std::string inputText_m, std::string inputText_l, int inputText_fontsize, std::string image_source, int inputPopupType, int yesNo_okCancel);
    void openResultPopUp(std::string inputText_h, std::string inputText_m, std::string inputText_l);
    void setProgress(float value);
    void setProgressText(std::string inputText);
    int getLayerViewFlags();
	void modelSelected(int);
	const std::unordered_set<GLModel*>& getSelectedModels();
	QVector2D world2Screen(QVector3D target);

    GLModel* findGLModelByName(QString filename);
	void connectShadow(GLModel* shadowModel);
    Q_INVOKABLE QString getVersion();
    Q_INVOKABLE void keyboardHandlerFocus();
	//gets center of selection in terms of world bound
    Q_INVOKABLE QVector3D getSelectedCenter();
	//gets lengths of volume containing all selected models
    Q_INVOKABLE QVector3D selectedModelsLengths();
    Q_INVOKABLE int getSelectedModelsSize();
    Q_INVOKABLE float getBedXSize();
    Q_INVOKABLE float getBedYSize();
    Q_INVOKABLE void setBedXSize(float x);
    Q_INVOKABLE void setBedYSize(float y);
    Q_INVOKABLE bool getGroupSelectionActive();
    Q_INVOKABLE void fixMesh();
    Q_INVOKABLE void disableObjectPickers();
    Q_INVOKABLE void enableObjectPickers();
    Q_INVOKABLE void setHandCursor();
    Q_INVOKABLE void setClosedHandCursor();
    Q_INVOKABLE void resetCursor();
    Q_INVOKABLE bool isSelected();
	Q_INVOKABLE bool isSelected(int ID);
	bool isSelected(GLModel* model);

    Q_INVOKABLE void selectPart(int ID);
    Q_INVOKABLE void unselectPart(int ID);
    Q_INVOKABLE void unselectAll();
    Q_INVOKABLE void modelVisible(int ID, bool isVisible);
    Q_INVOKABLE void doDelete();
    Q_INVOKABLE void doDeletebyID(int ID);
    Q_INVOKABLE void runArrange();
    Q_INVOKABLE void setViewMode(int viewMode);
    Q_INVOKABLE int getViewMode();
    Q_INVOKABLE void sendUpdateModelInfo();
    Q_INVOKABLE void backgroundClicked();
    Q_INVOKABLE void deleteList(int ID);
    Q_INVOKABLE void deleteSelectedModels();

    float selected_x_max();
    float selected_x_min();
    float selected_y_max();
    float selected_y_min();
    float selected_z_max();
    float selected_z_min();
	void modelMoveWithAxis(QVector3D axis, double distance);
	void modelMove(QVector3D displacement);
	void modelRotateWithAxis(const QVector3D& axis, double degree);
	QVector3D cameraViewVector();
private:

	GLModel* getModelByID(int ID);
    void unselectPartImpl(GLModel* target);
	bool groupSelectionActive = false;
    int viewMode;
    int layerViewFlags;
    int modelIDCounter;
    
	//TODO: get rid of this
	GLModel* _lastSelected;
	std::unordered_set<GLModel*> selectedModels;

	//Ray cast
	Hix::Input::RayCastController _rayCastController;
	Hix::UI::RotateXYZWidget _rotateWidget;
	Hix::UI::MoveXYZWidget _moveWidget;

signals:
    void updateModelInfo(int printing_time, int layer, QString xyz, float volume);
    void arrangeDone(std::vector<QVector3D>, std::vector<float>);


public slots:
	
    void sendUpdateModelInfo(int, int, QString, float);
    GLModel* createModelFile(Mesh* target_mesh, QString filename);
    void openModelFile(QString filename);
    void checkModelFile(GLModel* model);
    void deleteOneModelFile(int ID);
	void deleteOneModelFile(GLModel* model);

    void deleteModelFileDone();
    void deleteModelFile(int ID);
    void unDo();
    void reDo();
    void copyModel();
    void pasteModel();
    void groupSelectionActivate(bool);
    void runGroupFeature(int,QString, double, double, double, QVariant);
    bool multipleModelSelected(int ID);
    void lastModelSelected();
    void modelRotateByNumber(int axis, int, int, int);
    void modelMoveByNumber(int axis, int, int);
    void modelMoveInit();
    void modelMoveDone();
    void totalMoveDone();
    void modelRotateInit();
    void modelRotateDone();
    void totalRotateDone();
    void resetLayflat();
    void applyArrangeResult(std::vector<QVector3D>, std::vector<float>);
    void cleanselectedModel(int);
    void extensionSelect();
    void extensionUnSelect();
    void layFlatSelect();
    void layFlatUnSelect();
    void manualSupportSelect();
    void manualSupportUnselect();
    void openRotate();
    void closeRotate();
    void openMove();
    void closeMove();
    void openOrientation();
    void closeOrientation();
    void openSave();
    void closeSave();
    void save();
	void cameraViewChanged();
    void viewObjectChanged(bool checked);
    void viewSupportChanged(bool checked);
    void viewLayerChanged(bool checked);

    void layerInfillButtonChanged(bool chacked);
    void layerSupportersButtonChanged(bool chacked);
    void layerRaftButtonChanged(bool chacked);
};



QObject* FindItemByName(QList<QObject*> nodes, const QString& name);
QObject* FindItemByName(QQmlApplicationEngine* engine, const QString& name);



extern QmlManager *qmlManager;

#endif // QMLMANAGER_H
