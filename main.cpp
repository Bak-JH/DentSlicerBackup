#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlEngine>
#include <QQmlContext>
#include "utils/quaternionhelper.h"
#include "slicingengine.h"
#include "DentEngine/src/configuration.h"
#include "glmodel.h"
#include "qmlmanager.h"
#include <QQuickView>
#include "QtConcurrent/QtConcurrent"
#include "QFuture"
#include <QSplashScreen>
#include "utils/updatechecker.h"
//#include <QTranslator>

using namespace Qt3DCore;
QmlManager *qmlManager;


int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    /* Language patch
    QTranslator translator ;
    translator.load(":/lang_ko.qm");
    app.installTranslator(&translator);
    */

    QQmlApplicationEngine engine;
    qRegisterMetaType<vector<QVector3D>>("vector<QVector3D>");
    qRegisterMetaType<vector<float>>("vector<float>");

    QPixmap pixmap(":/Resource/splash2.png");

    QSplashScreen *splash = new QSplashScreen(pixmap);
    splash->show();


    QScopedPointer<QuaternionHelper> qq(new QuaternionHelper);
    qmlManager = new QmlManager();
    QScopedPointer<QmlManager> qm(qmlManager);


    engine.rootContext()->setContextProperty("qm", qm.data());
    //FindItemByName(&engine, "slicing_data")->setContextProperty("qm", qmlManager);
    engine.rootContext()->setContextProperty("qq",qq.data());
    //engine.rootContext()->setContextProperty("se",se.data());

    engine.load(QUrl(QStringLiteral("qrc:/Qml/main.qml")));

    // update module codes
    UpdateChecker* up = new UpdateChecker();
    up->checkForUpdates();
    //initWinSparkle();
    //checkForUpdates();


    //language patch
    //engine.retranslate();

    qmlManager->initializeUI(&engine);
    splash->close();

    //qmlManager->mainWindow->setProperty("visible",true);
    //qmlManager->loginWindow->setProperty("visible",true);


    return app.exec();
}
