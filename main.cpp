

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlEngine>
#include <QQmlContext>
#include "utils/quaternionhelper.h"
#include "slice/slicingengine.h"
#include "DentEngine/src/configuration.h"
#include "glmodel.h"
#include "qmlmanager.h"
#include <QQuickView>
#include "QtConcurrent/QtConcurrent"
#include "QFuture"
#include <QSplashScreen>
#include "utils/updatechecker.h"
#include "utils/gridmesh.h"
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
    qRegisterMetaType<std::vector<QVector3D>>("std::vector<QVector3D>");
    qRegisterMetaType<std::vector<float>>("std::vector<float>");
    qmlRegisterType<GridMesh>("DentSlicer", 1, 0, "GridMesh");
    //qmlRegisterType<SlicingConfiguration>("DentStudio", 1, 0, "SlicingConfiguration");

    /** Splash Image **/
    QPixmap pixmap(":/Resource/splash_dentslicer.png");
    QPainter painter(&pixmap);
    QPen penHText(QColor("#777777"));
    painter.setFont(QFont("Arial",9));
    painter.setPen(penHText);

    painter.drawText(QPoint(32,290), "Dental 3D Printing Solution");
    painter.drawText(QPoint(32,310), "Version " + version);
    //painter.drawText(QPoint(88,310), version);
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

#if  defined(QT_DEBUG) || defined(_DEBUG)
	qmlManager->mainWindow->setProperty("visible", true);
#else
	qmlManager->loginWindow->setProperty("visible", true);
#endif

    QSurfaceFormat format;
    format.setMajorVersion(4);
    format.setMinorVersion(3);
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
