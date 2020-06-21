

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

#include "glmodel.h"
#include <QQuickView>
#include "QtConcurrent/QtConcurrent"
#include "QFuture"
#include <QSplashScreen>
#include "utils/updatechecker.h"
#include "utils/gridmesh.h"
#include "qml/components/HixQML.h"
#include "qml/util/QMLUtil.h"
#include "application/ApplicationManager.h"
#include "utils/httpreq.h"

using namespace Qt3DCore;

#endif





#include <QApplication>
#include <QStringList>
#include <QTimer>
#include <QDebug>


#include <QOAuth2AuthorizationCodeFlow>
#include <QDesktopServices>
#include "auth/HixAuthHttpReply.h"
//
//#include "ytdemo.h"
//
//const char OPT_OAUTH_CODE[] = "-o";
//
//class Helper : public QObject {
//	Q_OBJECT
//
//public:
//	Helper() : QObject(), ytdemo_(this), waitForMsg_(false), msg_(QString()) {}
//
//public slots:
//	void run() {
//		connect(&ytdemo_, SIGNAL(linkingFailed()), this, SLOT(onLinkingFailed()));
//		connect(&ytdemo_, SIGNAL(linkingSucceeded()), this, SLOT(onLinkingSucceeded()));
//		connect(&ytdemo_, SIGNAL(channelInfoReceived()), this, SLOT(onChannelInfoReceived()));
//		connect(&ytdemo_, SIGNAL(channelInfoFailed()), this, SLOT(onChannelInfoFailed()));
//
//		ytdemo_.doOAuth(O2::GrantFlowAuthorizationCode);
//	}
//
//	void onLinkingFailed() {
//		qDebug() << "Linking failed!";
//		qApp->exit(1);
//	}
//
//	void onLinkingSucceeded() {
//		qDebug() << "Linking succeeded!";
//		ytdemo_.getUserChannelInfo();
//	}
//
//	void onChannelInfoFailed() {
//		qDebug() << "Error getting channel info!";
//		qApp->exit(1);
//	}
//
//	void onChannelInfoReceived() {
//		qDebug() << "Channel info received!";
//		qApp->quit();
//	}
//
//private:
//	YTDemo ytdemo_;
//	bool waitForMsg_;
//	QString msg_;
//};
//

const auto GOOGLE_CLIENT_ID = QString("89839571658-lte1h99n2hq3h44922g3ojj04hhtnr56.apps.googleusercontent.com");
const auto GOOGLE_APP_SECRET = QString("NpN4y7CwF8hzGPXj1k2nIlFE");
const auto GOOGLE_AUTH = QUrl("http://accounts.google.com/o/oauth2/auth");
const auto GOOGLE_TOKEN = QUrl("http://accounts.google.com/o/oauth2/token");



void auth(QApplication* app)
{

	auto google = new QOAuth2AuthorizationCodeFlow;

	google->setScope("email"); 
	google->setAuthorizationUrl(GOOGLE_AUTH);
	google->setClientIdentifier(GOOGLE_CLIENT_ID);
	google->setAccessTokenUrl(GOOGLE_TOKEN);
	google->setClientIdentifierSharedKey(GOOGLE_APP_SECRET);
	auto replyHandler = new HixAuthHttpReply(8000, app);
	//replyHandler->setCallbackBase("http://127.0.0.1:8000/accounts/google/login/callback/");
	replyHandler->setCallbackBase("http://127.0.0.1:%1/%2");
	//replyHandler->setCallbackPath("accounts/google/login/callback/");
	replyHandler->setCallbackPath("");
	google->setReplyHandler(replyHandler);
	//auto reply = google - &gt; get(QUrl("https://www.googleapis.com/plus/v1/people/me"));
	QObject::connect(google, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser,&QDesktopServices::openUrl);
	QObject::connect(google, &QOAuth2AuthorizationCodeFlow::granted, [=]() {
		qDebug() << __FUNCTION__ << __LINE__ << "Access Granted!";

		//auto reply = this->google->get(QUrl("https://www.googleapis.com/plus/v1/people/me"));
		//connect(reply, &QNetworkReply::finished, [reply]() {
		//	qDebug() << "REQUEST FINISHED. Error? " << (reply->error() != QNetworkReply::NoError);
		//	qDebug() << reply->readAll();
		});
	google->grant();



}

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

	QApplication  app(argc, argv);
	/* Language patch
	QTranslator translator ;
	translator.load(":/lang_ko.qm");
	app.installTranslator(&translator);
	*/
	app.setQuitOnLastWindowClosed(true);
	Hix::QML::registerTypes();
	qRegisterMetaType<std::vector<QVector3D>>("std::vector<QVector3D>");
	qRegisterMetaType<std::vector<float>>("std::vector<float>");


	auto& appManager = Hix::Application::ApplicationManager::getInstance();
	auto& engine = appManager.engine();
	engine.load(QUrl(QStringLiteral("qrc:/Qml/main.qml")));
	appManager.init();

	/** Splash Image **/
	QPixmap pixmap(":/Resource/splash_dentslicer.png");
	QPainter painter(&pixmap);
	QPen penHText(QColor("#777777"));
	painter.setFont(QFont("Arial", 9));
	painter.setPen(penHText);
	painter.drawText(QPoint(32, 290), "Dental 3D Printing Solution");
	painter.drawText(QPoint(32, 310), "Version " + Hix::Application::ApplicationManager::getInstance().getVersion());
	painter.drawText(QPoint(32, 330), "Developed by HiX Inc.");
	QSplashScreen* splash = new QSplashScreen(pixmap);
	splash->show();
	//login or make main window visible
	QQuickWindow *mainWindow;
	Hix::QML::getItemByID(appManager.getWindowRoot(), mainWindow, "window");
	QQuickWindow* loginWindow;
	QObject *loginButton;
	Hix::QML::getItemByID(appManager.getWindowRoot(), loginWindow, "loginWindow");
	Hix::QML::getItemByID(appManager.getWindowRoot(), loginButton, "loginButton");

	//Helper helper;
	//QTimer::singleShot(0, &helper, SLOT(run()));
	auth(&app);

#if  defined(QT_DEBUG) || defined(_DEBUG)
	mainWindow->setProperty("visible", true);
	loginWindow->close();
#else
	loginWindow->setProperty("visible", true);
	//auth
	httpreq* hr = new httpreq(loginWindow, loginButton);
	// update module codes
	UpdateChecker* up = new UpdateChecker();
	up->checkForUpdates();



#endif
	splash->close();
	return app.exec();
#endif

}

//#include "main.moc"
