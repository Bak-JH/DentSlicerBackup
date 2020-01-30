
#include "qmlmanager.h"
QmlManager* qmlManager;



#if defined(_DEBUG) || defined(QT_DEBUG )
//run unit test?
//#define _UNIT_TEST
#endif

#ifdef _UNIT_TEST
#include "unitTest/RunTests.h"
#else

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlEngine>
#include <QQmlContext>
#include "utils/quaternionhelper.h"
#include "slice/slicingengine.h"
#include "DentEngine/src/configuration.h"
#include "glmodel.h"
#include <QQuickView>
#include "QtConcurrent/QtConcurrent"
#include "QFuture"
#include <QSplashScreen>
#include "utils/updatechecker.h"
#include "utils/gridmesh.h"

using namespace Qt3DCore;

#endif

int main(int argc, char** argv)
{
#ifdef _UNIT_TEST

	std::string outputName = "unitTestCin.txt";

	Catch::ConfigData configData;
	configData.outputFilename = outputName;
	Catch::Session session; // There must be exactly one instance
	session.useConfigData(configData);
	int result = session.run(argc, argv);

	std::ifstream in(outputName);
	for (std::string line; std::getline(in, line); ) {
		qDebug() << QString::fromStdString(line);
	}

	//show 
	return result;

#else

	QApplication app(argc, argv);
	/* Language patch
	QTranslator translator ;
	translator.load(":/lang_ko.qm");
	app.installTranslator(&translator);
	*/
	qmlManager = new QmlManager();
	QScopedPointer<QmlManager> qm(qmlManager);
	QQmlApplicationEngine engine;
	qmlManager->engine = &engine;
	qRegisterMetaType<std::vector<QVector3D>>("std::vector<QVector3D>");
	qRegisterMetaType<std::vector<float>>("std::vector<float>");
	qmlRegisterType<GridMesh>("DentSlicer", 1, 0, "GridMesh");

	QScopedPointer<QuaternionHelper> qq(new QuaternionHelper);


	/** Splash Image **/
	QPixmap pixmap(":/Resource/splash_dentslicer.png");
	QPainter painter(&pixmap);
	QPen penHText(QColor("#777777"));
	painter.setFont(QFont("Arial", 9));
	painter.setPen(penHText);

	painter.drawText(QPoint(32, 290), "Dental 3D Printing Solution");
	painter.drawText(QPoint(32, 310), "Version " + qmlManager->getVersion());
	//painter.drawText(QPoint(88,310), version);
	painter.drawText(QPoint(32, 330), "Developed by HiX Inc.");

	QSplashScreen* splash = new QSplashScreen(pixmap);
	splash->show();

	engine.rootContext()->setContextProperty("qm", qm.data());
	engine.rootContext()->setContextProperty("qq", qq.data());

	engine.load(QUrl(QStringLiteral("qrc:/Qml/main.qml")));

	// update module codes
	UpdateChecker* up = new UpdateChecker();
	up->checkForUpdates();



	qmlManager->initializeUI();
	splash->close();

#if  defined(QT_DEBUG) || defined(_DEBUG)
	qmlManager->mainWindow->setProperty("visible", true);
#else
	qmlManager->loginWindow->setProperty("visible", true);
#endif

	//QSurfaceFormat format;
	//format.setMajorVersion(4);
	//format.setMinorVersion(3);
	//format.setProfile(QSurfaceFormat::CoreProfile);

	//QOpenGLContext gl_ctx;
	//gl_ctx.setFormat(format);
	//if (!gl_ctx.create())
	//	throw std::runtime_error("context creation failed");

	//QOffscreenSurface surface;
	//surface.create();
	//gl_ctx.makeCurrent(&surface);

	return app.exec();
#endif

}

