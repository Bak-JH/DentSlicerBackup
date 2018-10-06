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
    QString version = "1.0.0";

    /* Language patch
    QTranslator translator ;
    translator.load(":/lang_ko.qm");
    app.installTranslator(&translator);
    */

    QQmlApplicationEngine engine;
    qRegisterMetaType<vector<QVector3D>>("vector<QVector3D>");
    qRegisterMetaType<vector<float>>("vector<float>");

    /** Splash Image **/
    QPixmap pixmap(":/Resource/splash_final.png");
    QPainter painter(&pixmap);
    QPen penHText(QColor("#ffffff"));
    painter.setFont(QFont("Arial",9));
    painter.setPen(penHText);

    painter.drawText(QPoint(32,290), "Dental 3D Printing Solution");
    painter.drawText(QPoint(32,310), "Version");
    painter.drawText(QPoint(88,310), version);
    painter.drawText(QPoint(32,330), "Developed by HiX Inc.");

    QSplashScreen *splash = new QSplashScreen(pixmap);
    splash->show();


    QScopedPointer<QuaternionHelper> qq(new QuaternionHelper);
    qmlManager = new QmlManager();
    qmlManager->version = version;
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

    qmlManager->mainWindow->setProperty("visible",true);
    //qmlManager->loginWindow->setProperty("visible",true);

    QSurfaceFormat format;
    format.setMajorVersion(3);
    format.setMinorVersion(1);
    format.setProfile(QSurfaceFormat::CoreProfile);

    QOpenGLContext gl_ctx;
    gl_ctx.setFormat(format);
    if (!gl_ctx.create())
      throw std::runtime_error("context creation failed");

    QOffscreenSurface surface;
    surface.create();
    gl_ctx.makeCurrent(&surface);

    return app.exec();
}
