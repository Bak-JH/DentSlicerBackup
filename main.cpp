#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlEngine>
#include <QQmlContext>
#include "quaternionhelper.h"
#include "slicingengine.h"
#include "DentEngine/src/configuration.h"
#include "glmodel.h"
#include "qmlmanager.h"
#include <QQuickView>
#include "QtConcurrent/QtConcurrent"
#include "QFuture"
#include <QSplashScreen>

using namespace Qt3DCore;
QmlManager *qmlManager;

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QQmlApplicationEngine engine;
    qRegisterMetaType<vector<QVector3D>>("vector<QVector3D>");
    qRegisterMetaType<vector<float>>("vector<float>");

    QPixmap pixmap(":/Resource/splash2.png");

    QSplashScreen *splash = new QSplashScreen(pixmap);
    splash->show();

    QCursor cursorTarget1 = QCursor(QPixmap(":/resource/cursor.png"));
    QCursor cursorTarget2 = QCursor(QPixmap(":/resource/pen.png"));
    app.setOverrideCursor(cursorTarget1);
    //app.changeOverrideCursor(cursorTarget2);
    QScopedPointer<QuaternionHelper> qq(new QuaternionHelper);
    QScopedPointer<SlicingEngine> se(new SlicingEngine);
    qmlManager = new QmlManager();
    QScopedPointer<QmlManager> qm(qmlManager);
    QObject::connect(se.data(), SIGNAL(updateModelInfo(int,int,QString,float)), qmlManager, SLOT(sendUpdateModelInfo(int,int,QString,float)));

    //engine.rootContext()->setContextProperty("qm", qm.data());
    engine.rootContext()->setContextProperty("qm", qm.data());
    //FindItemByName(&engine, "slicing_data")->setContextProperty("qm", qmlManager);
    engine.rootContext()->setContextProperty("qq",qq.data());
    engine.rootContext()->setContextProperty("se",se.data());

    engine.load(QUrl(QStringLiteral("qrc:/Qml/main.qml")));

    qmlManager->initializeUI(&engine);
    //qmlManager->openModelFile("C:/Users/diridiri/Desktop/DLP/DLPslicer/partial2_flip.stl");//DLPslicer/partial2_flip.stl");
    splash->close();

    qmlManager->mainWindow->setProperty("visible",true);
    //qmlManager->loginWindow->setProperty("visible",true);

    return app.exec();
}

