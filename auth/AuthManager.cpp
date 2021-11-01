#include "AuthManager.h"
#include <stdexcept>
#include <QWebEngineView>
#include <QNetworkCookieJar>
#include <QWebEngineProfile>
#include <QWebEngineCookieStore>
#include <QtWebSockets/QWebSocket>
#include <QNetworkRequest>
#include <QQuickWindow>
#include <QWebEngineHistory>

#include <QWidget>
#include <QToolBar>
#include <QPushButton>
#include <QVBoxLayout>

#include "../application/ApplicationManager.h"
#include "../common/frozen/string.hpp"
#include "../../feature/FeaturesLoader.h"
#include "Qml/components/FeatureMenu.h"

#include <windows.h>
#include <Wincrypt.h>
using frozen::operator"" _fstr;

#ifdef _DEBUG
// #define DISABLE_AUTH
#endif
//constexpr auto HOST = "127.0.0.1"_fstr;
//constexpr auto PORT = "8000"_fstr;
//constexpr auto ADDRESS = HOST + ":" + PORT;
//constexpr bool IS_TLS = false;

constexpr auto HOST = "services.hix.co.kr"_fstr;
constexpr auto ADDRESS = HOST;
constexpr bool IS_TLS = true;

//constexpr auto WS_ADDRESS = IS_TLS ? HOST + ":3443"_fstr : HOST + ":3080"_fstr;


constexpr auto PROTOCOL = IS_TLS ? "https://"_fstr : "http://"_fstr;


constexpr auto LOGIN_URL = PROTOCOL + ADDRESS + "/product/login/"_fstr;
constexpr auto PROFILE_URL = LOGIN_URL + "?next=/product/profile/"_fstr;
constexpr auto LOGIN_REDIRECT_URL = PROTOCOL + ADDRESS + "/product/login_redirect/"_fstr;
constexpr auto REGISTER_SERIAL_URL = PROTOCOL + ADDRESS + "/product/register/"_fstr;
constexpr auto REGISTER_SERIAL_DONE_URL = PROTOCOL + ADDRESS + "/product/registration_done/"_fstr;
constexpr auto OWN_BASIC_URL = PROTOCOL + ADDRESS + "/product/owns/dentslicer"_fstr;
constexpr auto OWN_PRO_URL = PROTOCOL + ADDRESS + "/product/owns/dentslicer_pro"_fstr;


//constexpr auto WS_URL = "ws://"_fstr + ADDRESS + "/ws/product/dentslicer/"_fstr;
constexpr auto WS_PRO_URL = "wss://"_fstr + ADDRESS + "/ws/product/dentslicer_pro/"_fstr;
constexpr auto WS_BASIC_URL = "wss://"_fstr + ADDRESS + "/ws/product/dentslicer/"_fstr;


//constexpr auto TEST_URL = "http://"_fstr + ADDRESS + "/product/check_login/"_fstr;


Hix::Auth::WebViewWindow::WebViewWindow()
{
    _toolbar.reset(new QToolBar(this));
    _toolbar->addAction(QIcon(":/Resource/arrow_left.png"), "Back", [this]() { _webView->back(); });
    _toolbar->setStyleSheet("QToolBar {background: rgb(255, 255, 255)}");

    _layout.reset(new QVBoxLayout(this));
    _layout->addStretch();
    _layout->setSpacing(0);
    _layout->setContentsMargins(0, 0, 0, 0);
    _layout->addWidget(_toolbar.get());
}

void Hix::Auth::WebViewWindow::setWebView(QWebEngineView* webView)
{
    _webView.reset(webView);
    _layout->addWidget(_webView.get());

}

//TODO: fix this
void qDeleteLater(QWebEngineView* obj)
{
    obj->deleteLater();
}
void qDeleteLater(QWebSocket* obj)
{
    obj->deleteLater();
}

Hix::Auth::AuthManager::AuthManager() : _webView(nullptr, qDeleteLater), _ws(nullptr, qDeleteLater)
{
    //_webView->setPage(new HixWebviewPage());
    _ws.reset(new QWebSocket());

    QObject::connect(_ws.get(), &QWebSocket::connected,
        [this]() {
            unblockApp();
        });
    QObject::connect(_ws.get(), &QWebSocket::disconnected,
        [this]() {
            blockApp();
            _resumeWindow->setProperty("visible", true);
        });
    QObject::connect(_ws.get(), QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error),
        [this](QAbstractSocket::SocketError error) {
            blockApp();
            qDebug() << error;
            _resumeWindow->setProperty("visible", true);
        });
    QObject::connect(_ws.get(), QOverload<const QList<QSslError>&>::of(&QWebSocket::sslErrors),
        [this](const QList<QSslError>& errors) {
            blockApp();
            qDebug() << errors;
            _resumeWindow->setProperty("visible", true);
        });

    _manager.reset(new QNetworkAccessManager(this));
    QObject::connect(_manager.get(), &QNetworkAccessManager::finished, 
        [this](QNetworkReply* reply) {
            replyFinished(reply);
        });
}

void Hix::Auth::AuthManager::setResumeWindow(QObject* resume)
{
    _resumeWindow = resume;
    QObject::connect(_resumeWindow, SIGNAL(resume()), this, SLOT(resume()));

}

void Hix::Auth::AuthManager::setMainWindow(QQuickWindow* window)
{
    _mainWindow = window;
}
inline QNetworkCookie fromStdCk(const std::string& name, const std::string& val)
{

    auto ck = QNetworkCookie(QByteArray::fromStdString(name), QByteArray::fromStdString(val));
    ck.setDomain(HOST.data());
    //qDebug() << "cookie created: " << ck.name() << " " << ck.value() << " domain: " << ck.domain();
    return ck;
}

void Hix::Auth::AuthManager::setWebview(int width, int height)
{
    if (!_webViewWindow)
    {
        _webView.reset(new QWebEngineView());
        _webView->setMinimumWidth(width);
        _webView->setMinimumHeight(height);

        _webViewWindow.reset(new WebViewWindow());
        _webViewWindow->setWebView(_webView.get());
        _webViewWindow->setMinimumHeight(height);
        _webViewWindow->setMinimumWidth(width);
    }
}

void Hix::Auth::AuthManager::resume()
{
    login();
}

void Hix::Auth::AuthManager::acquireAuth(Hix::Settings::Liscense license)
{
    QNetworkRequest request;
    if (license == Hix::Settings::PRO)
    {
        request.setUrl(QUrl(WS_PRO_URL.data()));
    }
    else if (license == Hix::Settings::BASIC)
    {
        request.setUrl(QUrl(WS_BASIC_URL.data()));
    }

    QSslConfiguration config = QSslConfiguration::defaultConfiguration();
    config.setProtocol(QSsl::TlsV1_2OrLater);
    request.setSslConfiguration(config);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    std::unordered_set<std::string> ckNames = { "sessionid", "csrftoken", "messages" };
    std::string ckStr;
    for (auto& ck : _cks)
    {
        if (ckNames.find(ck.first) != ckNames.cend())
        {
            ckStr += ck.first + "=" + ck.second + ";";
        }

    }
    request.setRawHeader((QByteArray)"Cookie", QString::fromStdString(ckStr).toUtf8());
    _ws->open(request);
}




void Hix::Auth::AuthManager::login()
{
#ifdef DISABLE_AUTH
    auto& moddableSetting = Hix::Application::SettingsChanger::settings(Hix::Application::ApplicationManager::getInstance());
    moddableSetting.liscense = Hix::Settings::PRO;
    for (auto item : Hix::Application::ApplicationManager().getInstance().featureManager().featureItems()->childItems())
    {
        item->deleteLater();
    }
    auto menu = dynamic_cast<Hix::QML::FeatureMenu*>(Hix::Application::ApplicationManager::getInstance().featureManager().menu());
    Hix::Features::FeaturesLoader loader(&Hix::Application::ApplicationManager::getInstance().engine(), menu);
    loader.loadFeatureButtons();
    unblockApp();
    return;
#endif
    setWebview();

    //load json saved encrypted cookies from previous login
    auto cookieStore = _webView->page()->profile()->cookieStore();
    //cookieStore->deleteAllCookies();
    //_webView->history()->clear();
    QWebEngineProfile* defaultProfile = QWebEngineProfile::defaultProfile();
    //qDebug() << defaultProfile->cachePath();
    //qDebug() << defaultProfile->persistentStoragePath();
    
    //because google blocks QtWebEngine UserAgent
    //qDebug() << defaultProfile->httpUserAgent();
    auto defUA = defaultProfile->httpUserAgent().toStdString();
    size_t pos = defUA.find("QtWebEngine");
    if (pos != std::string::npos)
    {
        defUA.erase(pos, defUA.length());
    }
    defaultProfile->setHttpUserAgent(QString::fromUtf8(defUA.c_str(), defUA.length()));
    //qDebug() << defaultProfile->httpUserAgent();

    //loadCk(*cookieStore, sett);
    QObject::connect(cookieStore, &QWebEngineCookieStore::cookieAdded, [this](const QNetworkCookie& cookie) {
        //qDebug() << cookie.name() << cookie.value();
        _cks[cookie.name().toStdString()] = cookie.value().toStdString();
        });
    QObject::connect(_webView.get(), &QWebEngineView::urlChanged, [this](const QUrl& url) {
        //qDebug() << url;
        if (_webView)
        {
            if (url.toString().toStdString().find(LOGIN_REDIRECT_URL.to_std_string()) != std::string::npos)
            {
                //login success
                //_webView->close();
                //_webView.reset();
                QSslConfiguration config = QSslConfiguration::defaultConfiguration();
                config.setProtocol(QSsl::TlsV1_2OrLater);

                QNetworkRequest request(QUrl(OWN_PRO_URL.data()));
                request.setSslConfiguration(config);

                request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
                std::unordered_set<std::string> ckNames = { "sessionid", "csrftoken", "messages" };
                std::string ckStr;
                for (auto& ck : _cks)
                {
                    if (ckNames.find(ck.first) != ckNames.cend())
                        ckStr += ck.first + "=" + ck.second + ";";
                }
                request.setRawHeader((QByteArray)"Cookie", QString::fromStdString(ckStr).toUtf8());

                _manager->get(request);
            }

            /// before register product ///
            if (url.toString().toStdString().find(OWN_PRO_URL.to_std_string()) != std::string::npos)
            {
                _webView->page()->toPlainText([this](QString reply) {
                    if (reply == "owns product")
                    {
                        auto& moddableSetting = Hix::Application::SettingsChanger::settings(Hix::Application::ApplicationManager::getInstance());

                        qDebug() << "pro";
                        moddableSetting.liscense = Hix::Settings::PRO;
                        _webView->load(QUrl(REGISTER_SERIAL_DONE_URL.data()));
                    }
                    });
            }
            if (url.toString().toStdString().find(OWN_BASIC_URL.to_std_string()) != std::string::npos)
            {
                _webView->page()->toPlainText([this](QString reply) {
                    if (reply == "owns product")
                    {
                        auto& moddableSetting = Hix::Application::SettingsChanger::settings(Hix::Application::ApplicationManager::getInstance());

                        qDebug() << "basic";
                        moddableSetting.liscense = Hix::Settings::BASIC;
                        _webView->load(QUrl(REGISTER_SERIAL_DONE_URL.data()));
                    }
                });
            }

            if (url.toString().toStdString().find(REGISTER_SERIAL_DONE_URL.to_std_string()) != std::string::npos)
            {
                if (Hix::Application::ApplicationManager().getInstance().settings().liscense != Hix::Settings::NONE)
                {
                    acquireAuth(Hix::Application::ApplicationManager().getInstance().settings().liscense);

                    for (auto item : Hix::Application::ApplicationManager().getInstance().featureManager().featureItems()->childItems())
                    {
                        item->deleteLater();
                    }

                    auto menu = dynamic_cast<Hix::QML::FeatureMenu*>(Hix::Application::ApplicationManager::getInstance().featureManager().menu());
                    Hix::Features::FeaturesLoader loader(&Hix::Application::ApplicationManager::getInstance().engine(), menu);
                    loader.loadFeatureButtons();
                }
            }

            if (url.toString().toStdString().find(PROFILE_URL.to_std_string()) != std::string::npos)
            {
                _webView->load(QUrl(PROFILE_URL.data()));
            }
            
        }
        });
    _webView->load(QUrl(LOGIN_URL.data()));
	_webViewWindow->show();
    
}

void Hix::Auth::AuthManager::profile()
{
    //delete login cookies so user is promted to login again
    if (_webViewWindow) {
        _webView.reset();
        _webViewWindow->close();
        _webViewWindow.reset();
    }
    setWebview(995, 560);
    _webView->load(QUrl(PROFILE_URL.data()));
    _webViewWindow->show();
    if (!_webView){
        QObject::connect(_webView.get(), &QWebEngineView::urlChanged, [this](const QUrl& url) {
            if (url.toString().toStdString().compare(LOGIN_URL.to_std_string()) == 0 ||
                url.toString().toStdString().compare(LOGIN_REDIRECT_URL.to_std_string()) == 0)
            {
                auto cookieStore = _webView->page()->profile()->cookieStore();
                cookieStore->deleteAllCookies();

                //close webView window
                _webView.reset();
                _webViewWindow->close();
                _webViewWindow.reset();

                //block app and reopen login page
                blockApp();
                login();
            }
        });
    }
    _webView->page()->setBackgroundColor(Qt::transparent);
}  


void Hix::Auth::AuthManager::blockApp()
{
#ifdef DISABLE_AUTH
    return;
#endif
    _mainWindow->setProperty("visible", false);
}

void Hix::Auth::AuthManager::unblockApp()
{
    _mainWindow->setProperty("visible", true);
    if (_webViewWindow)
    {
        _webView.reset();
        _webViewWindow->close();
        _webViewWindow.reset();
    }

}



/// after register product ///
void Hix::Auth::AuthManager::replyFinished(QNetworkReply* reply)
{
    auto& moddableSetting = Hix::Application::SettingsChanger::settings(Hix::Application::ApplicationManager::getInstance());

    QString tempUrl = reply->url().toString();
    QString text = QString::fromUtf8(reply->readAll());

    qDebug() << tempUrl;
    qDebug() << text;

    if (tempUrl.toStdString().find(OWN_PRO_URL.to_std_string()) != std::string::npos)
    {
        if (text.toStdString().find("owns product") != std::string::npos)
        {
            qDebug() << "pro";
            moddableSetting.liscense = Hix::Settings::PRO;
            _webView->load(QUrl(REGISTER_SERIAL_DONE_URL.data()));
        }
        else
        {
            auto request = reply->request();
            request.setUrl(QUrl(OWN_BASIC_URL.data()));
            _manager->get(request);
        }
    }
    else if (tempUrl.toStdString().find(OWN_BASIC_URL.to_std_string()) != std::string::npos)
    {
        if (text.toStdString().find("owns product") != std::string::npos)
        {
            qDebug() << "basic";
            moddableSetting.liscense = Hix::Settings::BASIC;
            _webView->load(QUrl(REGISTER_SERIAL_DONE_URL.data()));
        }
        else
        {
            qDebug() << "none";
            moddableSetting.liscense = Hix::Settings::NONE;
            _webView->load(QUrl(REGISTER_SERIAL_URL.data()));
        }
    }
}   
