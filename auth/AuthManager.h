#pragma once
#include <memory>
#include <string>
#include <QMetaObject>
#include <unordered_map>
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
class QWebEngineView;
class QWebSocket;
class QQuickWindow;
namespace Hix
{
	namespace Settings
	{
		enum Liscense;
	}
	namespace Auth
	{

		class AuthManager: public QObject
		{
			Q_OBJECT
		public:
			AuthManager();
			void setMainWindow(QQuickWindow* window);
			void setResumeWindow(QObject* resume);

			void login();
			void profile();

		private:
			void setWebview(int width = 720, int height = 720);
			void acquireAuth(Hix::Settings::Liscense license);
			void blockApp();
			void unblockApp();
			std::string guid()const;

			//we need custom deleters with these pointers since we use deleteLater
			//std::unique_ptr<QWebEngineView, void(QObject::*)(QWebEngineView)> _webView;
			//std::unique_ptr<QWebSocket, void(QObject::*)(QWebSocket)> _ws;
			std::unique_ptr<QWebEngineView, void(*)(QWebEngineView*)> _webView;
			std::unique_ptr<QWebSocket, void(*)(QWebSocket*)> _ws;
			std::unique_ptr<QNetworkAccessManager> _manager;
			QMetaObject::Connection _ckAddedConnToken;
			QQuickWindow* _mainWindow;
			QObject* _resumeWindow;
			std::unordered_map<std::string, std::string> _cks;
		public slots:
			void replyFinished(QNetworkReply* reply); 

		private slots:
			void resume();

		};


	}
}
