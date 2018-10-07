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
//#include <windows.h>
//#include <dbghelp.h>
//#include <QQuickItem>

using namespace Qt3DCore;
QmlManager *qmlManager;

//void exceptionFilter()
//{
//    QString applicationPath = QApplication::applicationDirPath();
//    QString logPath = applicationPath + QString("/log/");
//    QDir dir(logPath);
//    dir.mkpath(logPath);
//     QString time = QDateTime::currentDateTime().toString(Qt::ISODate);
//    QString logFileName = logPath + QString("error") + "[" + time + "]" + QString(".log");
//    QFile outFile(logFileName);
//    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
//    QTextStream ts(&outFile);
//     void         * stack[15];
//    unsigned short frames;
//    SYMBOL_INFO    symbol;
//    HANDLE         process;
//     process = GetCurrentProcess();
//    SymInitialize( process, NULL, TRUE ); // Error
//     frames = CaptureStackBackTrace( 0, 15, stack, NULL );
//     for(int i = 0; i < frames; i++ )
//    {
//        SymFromAddr( process, ( DWORD64 )( stack[ i ] ), 0, &symbol ); // Error
//         ts << symbol.Name << endl;
//    }
//    outFile.close();
//    exit(-1);
//    qApp->quit();
//}
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
