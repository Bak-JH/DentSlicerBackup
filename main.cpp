#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlEngine>
#include <QQmlContext>
#include "quaternionhelper.h"


int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QScopedPointer<QuaternionHelper> qq(new QuaternionHelper);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    engine.rootContext()->setContextProperty("qq",qq.data());



    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
