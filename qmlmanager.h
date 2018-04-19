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
#include <QQmlProperty>0269
#include "feature/autoarrange.h"
#include "feature/shelloffset.h"
#include "glmodel.h"
#include "QtConcurrent/QtConcurrentRun"
#include "QFuture"
#include "utils/httpreq.h"

class QmlManager : public QObject
{
    Q_OBJECT
public:
    explicit QmlManager(QObject *parent = nullptr);
    QQmlApplicationEngine* engine;

    // UI components
    QObject* mainWindow;
    QObject* loginWindow;
    QObject* loginButton;
    QEntity* models;
    QObject* mv;
    // model rotate components
    QObject *rotatePopup;
    QObject *rotateSphereobj;
    Qt3DCore::QEntity *rotateSphere;
    Qt3DCore::QEntity *rotateSphereX;
    Qt3DCore::QEntity *rotateSphereY;
    Qt3DCore::QEntity *rotateSphereZ;

    // model move components
    QObject *movePopup;
    Qt3DCore::QEntity *managerModel;
    Qt3DCore::QEntity *moveArrow;
    Qt3DCore::QEntity *moveArrowX;
    Qt3DCore::QEntity *moveArrowY;
    QObject *moveArrowobj;

    // model layflat components;
    QObject *layflatPopup;
    QObject *layflatButton;

    QObject* partList;

    // model cut components
    QObject *cutPopup;
    QObject *curveButton;
    QObject *flatButton;
    QObject *cutSlider;

    // labelling components
    QObject *text3DInput;
    QObject *labelPopup;
    QObject *labelFontBox;

    // orientation components
    QObject* orientPopup;
    QObject* progress_popup;
    QObject* result_popup;
    QObject* orientButton;
    // extension components
    QObject* extensionPopup;
    QObject* extensionButton;
    // shell offset components;
    QObject* shelloffsetPopup;

    // auto repair components
    QObject* repairPopup;
    QObject* repairButton;

    // auto arrange components
    QObject* arrangePopup;

    // save components
    QObject* saveButton;

    // export components
    //QObject* exportButton;
    QObject* exportOKButton;

    vector<GLModel*> glmodels;
    GLModel* selectedModel = nullptr;


    int groupFunctionIndex;
    QString groupFunctionState;
    float progress = 0;
    void showRotateSphere();
    void showMoveArrow();
    void hideRotateSphere();
    void hideMoveArrow();
    void mouseHack();
    void initializeUI(QQmlApplicationEngine *e);
    void openModelFile_internal(QString filename);
    void runArrange_internal();
    void disconnectHandlers(GLModel* glmodel);
    void connectHandlers(GLModel* glmodel);

    void addPart(QString fileName, int ID);
    void deletePart(int ID);
    void openProgressPopUp();
    void openResultPopUp(string inputText_h, string inputText_m, string inputText_l);
    void setProgress(float value);
    void setProgressText(string inputText);

    Q_INVOKABLE bool isSelected();
    Q_INVOKABLE void selectPart(int ID);
    Q_INVOKABLE void unselectPart(int ID);
    Q_INVOKABLE void modelVisible(int ID, bool isVisible);
    Q_INVOKABLE void doDelete();


private:
    //bool glmodels_arranged;



signals:
    void updateModelInfo(int printing_time, int layer, QString xyz, float volume);
    void arrangeDone(vector<QVector3D>, vector<float>);


public slots:
    void sendUpdateModelInfo(int, int, QString, float);
    void createModelFile(Mesh* target_mesh, QString filename);
    void openModelFile(QString filename);
    void deleteModelFile(int ID);
    void runGroupFeature(int,QString);
    void modelSelected(int);
    void modelRotate(int,int);

    void modelRotateByNumber(int axis, int, int, int);
    void modelMove(int,int);
    void modelMoveByNumber(int axis, int, int);
    void modelMoveDone(int);
    void modelRotateDone(int);
    void resetLayflat();
    void runArrange();
    void applyArrangeResult(vector<QVector3D>, vector<float>);
    void cleanSelectedModel(int);
    void extensionSelect();
    void extensionUnSelect();

};



QObject* FindItemByName(QList<QObject*> nodes, const QString& name);
QObject* FindItemByName(QQmlApplicationEngine* engine, const QString& name);



extern QmlManager *qmlManager;

#endif // QMLMANAGER_H
