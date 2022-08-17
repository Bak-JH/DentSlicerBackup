#include <boost/stacktrace.hpp>
#include <fstream>
#include <QNetworkReply>

#include "application/ApplicationManager.h"
#include "utils/CrashReport/version.h"
#include "sendError.h"

#include "utils/CrashReport/version.h"

using namespace Hix;
using namespace Hix::Render;
using namespace Hix::Features;
using namespace Hix::Engine3D;




Hix::Features::SendError::SendError(const std::string& title, const std::string& details)
{

}

Hix::Features::SendError::~SendError()
{
}

void Hix::Features::SendError::run()
{
	postUIthread([]() {
		//std::fstream file("D:\\err.txt", std::fstream::out);
		//file << boost::stacktrace::to_string(boost::stacktrace::stacktrace());
		auto stack = boost::stacktrace::to_string(boost::stacktrace::stacktrace());

		QNetworkAccessManager* manager = new QNetworkAccessManager();
		const QUrl url(CRASHREPORTER_SUBMIT_URL);
		QNetworkRequest request(url);
		request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

		QJsonObject obj;
		obj["build_id"] = BUILD_ID;
		obj["version"] = VER_FILEVERSION_STR;
		obj["call-stack"] = QString::fromStdString(stack);

		QJsonDocument doc(obj);
		QByteArray data = doc.toJson();

		QNetworkReply* reply = manager->post(request, data);

		QObject::connect(reply, &QNetworkReply::finished, [=]() {
			if (reply->error() == QNetworkReply::NoError) {
				QString contents = QString::fromUtf8(reply->readAll());
				qDebug() << contents;
			}
			else {
				QString err = reply->errorString();
				qDebug() << reply->error();
			}
			reply->deleteLater();
			});



		auto& progressManager = Hix::Application::ApplicationManager::getInstance().taskManager().progressManager();
		progressManager.deletePopup();
		});

}
