#pragma once
#include <memory>
#include <string>
#include <QMetaObject>
class QWebEngineView;
class QWebSocket;
class QQuickWindow;
namespace Hix
{
	namespace Auth
	{
		class AuthManager
		{
		public:
			AuthManager();
			void setMainWindow(QQuickWindow* window);
			void acquireAuth();
		private:
			void login();
			void logout();
			void blockApp();
			void unblockApp();
			void clearSavedCks();
			std::string guid()const;

			//we need custom deleters with these pointers since we use deleteLater
			//std::unique_ptr<QWebEngineView, void(QObject::*)(QWebEngineView)> _webView;
			//std::unique_ptr<QWebSocket, void(QObject::*)(QWebSocket)> _ws;
			std::unique_ptr<QWebEngineView, void(*)(QWebEngineView*)> _webView;
			std::unique_ptr<QWebSocket, void(*)(QWebSocket*)> _ws;
			QMetaObject::Connection _ckAddedConnToken;
			QQuickWindow* _mainWindow;
		};


	}
}
