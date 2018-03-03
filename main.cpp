#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlEngine>
#include <QQmlContext>
#include "quaternionhelper.h"
#include "slicingengine.h"
#include "configuration.h"
#include "glmodel.h"
#include "qmlmanager.h"
#include "lights.h"
#include <QQuickView>


using namespace Qt3DCore;

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QCursor cursorTarget1 = QCursor(QPixmap(":/resource/cursor.png"));
    QCursor cursorTarget2 = QCursor(QPixmap(":/resource/pen.png"));
    app.setOverrideCursor(cursorTarget1);
    //app.changeOverrideCursor(cursorTarget2);
    QScopedPointer<QuaternionHelper> qq(new QuaternionHelper);
    QScopedPointer<SlicingEngine> se(new SlicingEngine);
    QmlManager *qmlManager = new QmlManager();
    QScopedPointer<QmlManager> qm(qmlManager);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    engine.rootContext()->setContextProperty("qm", qm.data());
    engine.rootContext()->setContextProperty("qq",qq.data());
    engine.rootContext()->setContextProperty("se",se.data());

    qmlManager->initializeUI(&engine);

    QObject::connect(se.data(), SIGNAL(updateModelInfo(int,int,QString,float)), qm.data(), SLOT(sendUpdateModelInfo(int,int,QString,float)));

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}

