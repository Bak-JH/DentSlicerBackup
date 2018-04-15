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
#include <QQmlProperty>0269
#include "feature/autoarrange.h"
#include "feature/shelloffset.h"
#include "glmodel.h"
#include "QtConcurrent/QtConcurrentRun"
#include "QFuture"

class QmlManager : public QObject
{
    Q_OBJECT
public:
    explicit QmlManager(QObject *parent = nullptr);
    QQmlApplicationEngine* engine;

    // UI components
    QObject* mainWindow;
    QEntity* models;
    QObject* mv;

    // model cut components
    QObject *cutPopup;
    QObject *curveButton;
    QObject *flatButton;
    QObject *slider;

    // labelling components
    QObject *text3DInput;
    QObject *labelPopup;
    QObject *labelFontBox;

    // orientation components
    QObject* orientPopup;
    QObject* progress_popup;
    QObject* result_popup;

    // extension components
    QObject* extensionPopup;

    // shell offset components;
    QObject* shelloffsetPopup;

    // auto repair components
    QObject* repairPopup;

    // auto arrange components
    QObject* arrangePopup;

    // save components
    QObject* saveButton;

    // export components
    //QObject* exportButton;
    QObject* exportOKButton;

    vector<GLModel*> glmodels;
    GLModel* selectedModel = nullptr;

    Qt3DCore::QEntity *managerModel;
    QObject *rotateSphereobj;
    Qt3DCore::QEntity *rotateSphere;
    Qt3DCore::QEntity *rotateSphereX;
    Qt3DCore::QEntity *rotateSphereY;
    Qt3DCore::QEntity *rotateSphereZ;
    Qt3DCore::QEntity *moveArrow;
    Qt3DCore::QEntity *moveArrowX;
    Qt3DCore::QEntity *moveArrowY;
    QObject *moveArrowobj;

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

    void openProgressPopUp();
    void openResultPopUp(string inputText_h, string inputText_m, string inputText_l);
    void setProgress(float value);
    void setProgressText(string inputText);

    Q_INVOKABLE void modelVisible(int ID, bool isVisible);
    Q_INVOKABLE void doDelete();

private:
    bool glmodels_arranged;



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
    void modelMove(int,int);
    void modelMoveDone(int);
    void modelRotateDone(int);
    void runArrange();
    void applyArrangeResult(vector<QVector3D>, vector<float>);

};



QObject* FindItemByName(QList<QObject*> nodes, const QString& name);
QObject* FindItemByName(QQmlApplicationEngine* engine, const QString& name);



extern QmlManager *qmlManager;

#endif // QMLMANAGER_H
