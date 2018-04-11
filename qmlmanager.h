#ifndef QMLMANAGER_H
#define QMLMANAGER_H

#include <QObject>
#include <QDebug>
#include <QString>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlEngine>
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
    QObject* progress_text;

    // auto repair components
    QObject* repairPopup;

    // auto arrange components
    QObject* arrangePopup;

    // save components
    QObject* saveButton;

    // export components
    QObject* exportButton;

    vector<GLModel*> glmodels;
    GLModel* selectedModel = nullptr;

    Qt3DCore::QEntity *managerModel;
    QObject *rotateSphereobj;
    Qt3DCore::QEntity *rotateSphere;
    Qt3DCore::QEntity *rotateSphereX;
    Qt3DCore::QEntity *rotateSphereY;
    Qt3DCore::QEntity *rotateSphereZ;
    Qt3DCore::QEntity *moveArrow;
    QObject *moveArrowobj;
    void showRotateSphere();
    void showMoveArrow();
    void initializeUI(QQmlApplicationEngine *e);
    void openModelFile_internal(QString filename);
    void runArrange_internal();
    void disconnectHandlers(GLModel* glmodel);
    void connectHandlers(GLModel* glmodel);

    Q_INVOKABLE void modelVisible(int ID, bool isVisible);
    Q_INVOKABLE void doDelete();

private:
    bool glmodels_arranged;



signals:
    void updateModelInfo(int printing_time, int layer, QString xyz, float volume);
    void arrangeDone(vector<QVector3D>, vector<float>);


public slots:
    void sendUpdateModelInfo(int, int, QString, float);
    void openModelFile(QString filename);
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



extern QmlManager qm;

#endif // QMLMANAGER_H
