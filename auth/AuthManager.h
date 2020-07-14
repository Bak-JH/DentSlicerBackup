#pragma once
#include <memory>
#include <string>
#include <QMetaObject>
#include <unordered_map>
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
			void login();
			void logout();

		private:
			void setWebview();
			void acquireAuth();
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
			std::unordered_map<std::string, std::string> _cks;
		};


	}
}
