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
#include "feature/shelloffset.h"
#include "glmodel.h"
#include "QtConcurrent/QtConcurrentRun"
#include "QFuture"
#include "utils/httpreq.h"
#include <QKeyboardHandler>
#include "input/raycastcontroller.h"
#include "feature/overhangDetect.h"
#include "Tasking/TaskManager.h"
#include "slice/SlicingOptBackend.h"
#include "support/SupportRaftManager.h"
#include "feature/FeatureHistoryManager.h"
#include "feature/interfaces/Feature.h"
#include "widget/GridBed.h"
#include "Settings/AppSetting.h"
#define VIEW_MODE_OBJECT 0
#define VIEW_MODE_LAYER 2

#define LAYER_INFILL 0x01
#define LAYER_SUPPORTERS 0x02
#define LAYER_RAFT 0x04


/// PLZ DELETE THESE DISGUSTING THINGS ///
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


namespace Hix
{
	namespace Features
	{
		class Feature;
	}
    namespace QML
    {
        class FeatureMenu;
    }
}
class QQuickItem;
class QmlManager : public QObject
{
    Q_OBJECT
public:

	template <typename F>
	static void postToObject(F&& fun, QObject* obj = qApp) {
		QMetaObject::invokeMethod(obj, std::forward<F>(fun));
	}

	template<typename FeatureType>
	bool isActive()
	{
		if (_currentMode.get() != nullptr)
		{
			return dynamic_cast<const FeatureType*>(_currentMode.get()) != nullptr;
		}
		return false;
	}
	bool isFeatureActive();
	template <typename F>
	static void postToThread(F&& fun, QThread* thread = qApp->thread()) {
		auto* obj = QAbstractEventDispatcher::instance(thread);
		Q_ASSERT(obj);
		QMetaObject::invokeMethod(obj, std::forward<F>(fun));
	}


    explicit QmlManager(QObject *parent = nullptr);
    QQmlApplicationEngine* engine;

    // UI components
	Qt3DRender::QCamera* _camera;
    QQuickItem* featureArea;
    QQuickItem* popupArea;
    QObject* mainWindow;
    QObject* loginWindow;
    QObject* loginButton;
    QObject* boxUpperTab;
    QObject* boxLeftTab;
	QQuickItem* scene3d;
    Qt3DCore::QEntity* models;
	Qt3DCore::QEntity* total;
    Qt3DCore::QTransform* systemTransform;
	Qt3DCore::QEntity* mv;
	Qt3DCore::QEntity* boundedBox;
    Qt3DCore::QEntity *mttab;
    QObject* undoRedoButton;
    QObject* slicingData;
	QObject* ltso;

    Qt3DInput::QKeyboardHandler* keyboardHandler;

    // model rotate components
    QObject *rotatePopup;



    // selection popup
    QObject* yesno_popup;
    QObject* result_popup;


	QQuickItem* partList;



    std::unordered_map<GLModel*, std::unique_ptr<GLModel>> glmodels;

    std::vector<size_t> copyMeshes;

	//!
    int rotateSnapAngle = 0;
    int rotateSnapStartAngle = 0;
    int rotateSnapQuotient = 0;
    //bool moveActive = false;

	Hix::Input::RayCastController& getRayCaster();
    QString groupFunctionState;
    int groupFunctionIndex;
    float progress = 0;
    void initializeUI();
    void openModelFile_internal(QString filename);
    void openArrange();
    void addPart(QString fileName, int ID);
    void deletePartListItem(int ID);
    void openProgressPopUp();
    void openYesNoPopUp(bool selectedList_vis, std::string inputText_h, std::string inputText_m, std::string inputText_l, int inputText_fontsize, std::string image_source, int inputPopupType, int yesNo_okCancel);
    void openResultPopUp(std::string inputText_h, std::string inputText_m, std::string inputText_l);
    void setProgress(float value);
    void setProgressText(std::string inputText);
    int getLayerViewFlags();
	void modelSelected(int);
	
	Hix::Features::Mode* currentMode()const;
	//TODO: temp
	void setMode(Hix::Features::Mode*);

	const std::unordered_set<GLModel*>& getSelectedModels();
	const std::unordered_set<GLModel*>& getAllModels();

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
    Q_INVOKABLE bool getGroupSelectionActive();
    Q_INVOKABLE void fixMesh();
    Q_INVOKABLE void disableObjectPickers();
    Q_INVOKABLE void enableObjectPickers();
    Q_INVOKABLE void setHandCursor();
	Q_INVOKABLE void setEraserCursor();
    Q_INVOKABLE void setClosedHandCursor();
    Q_INVOKABLE void resetCursor();
	Q_INVOKABLE void settingFileChanged(QString path);

    Q_INVOKABLE bool isSelected();
	Q_INVOKABLE bool isSelected(int ID);
	bool isSelected(GLModel* model);
    void showCubeWidgets(GLModel* model);
	void addSupport(std::unique_ptr<Hix::Features::FeatureContainer> container);

    Q_INVOKABLE void selectPart(int ID);
    Q_INVOKABLE void unselectPart(int ID);
    Q_INVOKABLE void unselectAll();
    Q_INVOKABLE void modelVisible(int ID, bool isVisible);
    Q_INVOKABLE void doDelete();
    Q_INVOKABLE void setViewMode(int viewMode);
    Q_INVOKABLE int getViewMode();
    Q_INVOKABLE void sendUpdateModelInfo();
    Q_INVOKABLE void backgroundClicked();
    Q_INVOKABLE void deleteList(int ID);
    Q_INVOKABLE void deleteSelectedModels();

	Hix::Engine3D::Bounds3D getSelectedBound()const;
	void modelMoveWithAxis(QVector3D axis, double distance);
	void modelMove(QVector3D displacement);
	QVector3D cameraViewVector();
	Hix::Tasking::TaskManager& taskManager();
	Hix::Support::SupportRaftManager& supportRaftManager();
	Hix::Features::FeatureHisroyManager& featureHistoryManager();
	Hix::Features::Mode* getCurrentMode();
	void unselectPart(GLModel* target);
	void addToGLModels(GLModel* target);
	void addToGLModels(std::unique_ptr<GLModel>&& target);
	const Hix::Settings::AppSetting& settings()const;
private:
	Hix::Tasking::TaskManager _taskManager;
	GLModel* getModelByID(int ID);
	bool groupSelectionActive = false;
    int viewMode;
    int layerViewFlags;
	//TODO: get rid of this
	GLModel* _lastSelected;
	std::unordered_set<GLModel*> selectedModels;

	SlicingOptBackend _optBackend;
	//Ray cast
	Hix::Input::RayCastController _rayCastController;

	//cursors
	QCursor _cursorEraser;
	Hix::Support::SupportRaftManager _supportRaftManager;
	std::unique_ptr<Hix::Features::Mode> _currentMode;
	Hix::Features::FeatureHisroyManager _featureHistoryManager;
	Hix::Settings::AppSetting _setting;
	Hix::UI::GridBed _bed;
signals:
    void updateModelInfo(int printing_time, int layer, QString xyz, float volume);
    void arrangeDone(std::vector<QVector3D>, std::vector<float>);


public slots:
	// model cut popup codes
	void modelCut();
	void cutModeSelected(int);
	void openCut();
	void closeCut();



	void stateChangeLabelling();
	void generateLabelMesh();

	void openExtension();
	void closeExtension();

	void openLayFlat();
	void closeLayFlat();
	void generateLayFlat();

	void getSliderSignal(double);
	void getCrossSectionSignal(int);
	void modelRepair();

    void sendUpdateModelInfo(int, int, QString, float);
    void openModelFile();
	std::unique_ptr<GLModel> removeFromGLModels(GLModel* target);
	GLModel* releaseFromGLModels(GLModel* target);

    void deleteModelFileDone();
    void deleteModelFile(GLModel* target);
    void unDo();
    void reDo();
    void copyModel();
    void pasteModel();
    void groupSelectionActivate(bool);
    void runGroupFeature(int,QString, double, double, double, QVariant);
    bool multipleModelSelected(int ID);
    void applyRotation(int mode, qreal, qreal, qreal);
    void applyMove(int axis, qreal, qreal);

	void totalScaleDone();

    void cleanselectedModel(int);
   
    void openRotate();
    void closeRotate();
    void openMove();
    void closeMove();

	void openScale();
	void closeScale();
	void applyScale(double, double, double);

    void openOrientation();
    void closeOrientation();
	void cameraViewChanged();
    void viewObjectChanged(bool checksed);
    void viewLayerChanged(bool checked);

	void openSupport();
	void closeSupport();
	void clearSupports();
	void supportEditEnabled(bool enabled);
	void supportCancelEdit();
	void supportApplyEdit();
	void generateAutoSupport();
	void regenerateRaft();

    void layerInfillButtonChanged(bool chacked);
    void layerSupportersButtonChanged(bool chacked);
    void layerRaftButtonChanged(bool chacked);


	void generateShellOffset(double factor);

	//modelbuilder
	void openModelBuilder();
	void closeModelBuilder();
	void buildModel();
	void mbRangeSliderValueChangedFirst(double value);
	void mbRangeSliderValueChangedSecond(double value);


};



QObject* FindItemByName(QList<QObject*> nodes, const QString& name);
QObject* FindItemByName(QQmlApplicationEngine* engine, const QString& name);



extern QmlManager *qmlManager;

#endif //QMLMANAGER_H
