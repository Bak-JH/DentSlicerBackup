
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
#include "qml/components/HixQML.h"
#include "application/ApplicationManager.h"
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
	Hix::QML::registerTypes();
	qRegisterMetaType<std::vector<QVector3D>>("std::vector<QVector3D>");
	qRegisterMetaType<std::vector<float>>("std::vector<float>");


	/** Splash Image **/
	QPixmap pixmap(":/Resource/splash_dentslicer.png");
	QPainter painter(&pixmap);
	QPen penHText(QColor("#777777"));
	painter.setFont(QFont("Arial", 9));
	painter.setPen(penHText);
	painter.drawText(QPoint(32, 290), "Dental 3D Printing Solution");
	painter.drawText(QPoint(32, 310), "Version " + qmlManager->getVersion());
	painter.drawText(QPoint(32, 330), "Developed by HiX Inc.");
	QSplashScreen* splash = new QSplashScreen(pixmap);
	splash->show();
	auto& engine = Hix::Application::ApplicationManager::getInstance().engine();
	engine.load(QUrl(QStringLiteral("qrc:/Qml/main.qml")));
	qmlManager = new QmlManager();
	// update module codes
	UpdateChecker* up = new UpdateChecker();
	up->checkForUpdates();
	splash->close();

#if  defined(QT_DEBUG) || defined(_DEBUG)
	qmlManager->mainWindow->setProperty("visible", true);
#else
	qmlManager->loginWindow->setProperty("visible", true);
#endif
	return app.exec();
#endif

}

