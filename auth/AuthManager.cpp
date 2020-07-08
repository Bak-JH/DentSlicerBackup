#include "AuthManager.h"
#include <stdexcept>
#include <QWebEngineView>
#include <QNetworkCookieJar>
#include <QWebEngineProfile>
#include <QWebEngineCookieStore>
#include <QtWebSockets/QWebSocket>
#include <QNetworkRequest>
#include <QQuickWindow>
#include "../application/ApplicationManager.h"
#include "../common/frozen/string.hpp"

#include <windows.h>
#include <Wincrypt.h>
using frozen::operator"" _fstr;

constexpr auto HOST = "127.0.0.1"_fstr;
constexpr auto PORT = "8000"_fstr;
constexpr auto ADDRESS = HOST + ":" + PORT;


constexpr auto LOGIN_URL = "http://"_fstr + ADDRESS + "/product/login/"_fstr;
constexpr auto LOGIN_REDIRECT_URL = "http://"_fstr + ADDRESS + "/product/login_redirect/"_fstr;
constexpr auto REGISTER_SERIAL_URL = "http://"_fstr + ADDRESS + "/product/register/dentslicer"_fstr;
constexpr auto REGISTER_SERIAL_DONE_URL = "http://"_fstr + ADDRESS + "/product/registration_done/"_fstr;


constexpr auto WS_URL = "ws://"_fstr + ADDRESS + "/ws/product/dentslicer/"_fstr;
//constexpr auto TEST_URL = "http://"_fstr + ADDRESS + "/product/check_login/"_fstr;

//seems on windows, webview stores cookies via chromium
//std::string encryptStr(const std::string& value);
//std::string decryptStr(const std::string& value);
//void storeCk(const QNetworkCookie& ck, Hix::Settings::AdditionalSetting& json);
//void loadCk(QWebEngineCookieStore& jar, const Hix::Settings::AdditionalSetting& json);
//void loadCk(QList<QNetworkCookie>& jar, const Hix::Settings::AdditionalSetting& json);

//class HixWebviewPage : public QWebEnginePage
//{
//public:
//    using QWebEnginePage::QWebEnginePage;
//protected:
//    bool acceptNavigationRequest(const QUrl& url, QWebEnginePage::NavigationType type, bool isMainFrame)override
//    {
//        qDebug() << url;
//        return true;
//    }
//
//};
//

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
            //unblockApp();
        });
    QObject::connect(_ws.get(), &QWebSocket::disconnected,
        [this]() {
            blockApp();
            login();
        });
    QObject::connect(_ws.get(), QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error),
        [this](QAbstractSocket::SocketError error) {
            blockApp();
            login();
        });
}

void Hix::Auth::AuthManager::setMainWindow(QQuickWindow* window)
{
    _mainWindow = window;
}
inline QNetworkCookie fromStdCk(const std::string& name, const std::string& val)
{

    auto ck = QNetworkCookie(QByteArray::fromStdString(name), QByteArray::fromStdString(val));
    ck.setDomain(HOST.data());
    qDebug() << "cookie created: " << ck.name() << " " << ck.value() << " domain: " << ck.domain();
    return ck;
}

void Hix::Auth::AuthManager::acquireAuth()
{
    auto& sett = Hix::Application::ApplicationManager::getInstance().settings().additionalSetting;

    QNetworkRequest request(QUrl(WS_URL.data()));
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
    auto& sett = Hix::Application::ApplicationManager::getInstance().settings().additionalSetting;
    _webView.reset(new QWebEngineView(nullptr));

    //load json saved encrypted cookies from previous login
    auto cookieStore = _webView->page()->profile()->cookieStore();
    //cookieStore->deleteAllCookies();
    //loadCk(*cookieStore, sett);
    QObject::connect(cookieStore, &QWebEngineCookieStore::cookieAdded, [this](const QNetworkCookie& cookie) {
        qDebug() << cookie.name() << cookie.value();
        _cks[cookie.name().toStdString()] = cookie.value().toStdString();
        auto& moddableSetting = Hix::Application::SettingsChanger::settings(Hix::Application::ApplicationManager::getInstance()).additionalSetting;
        //storeCk(cookie, moddableSetting);
        });
    QObject::connect(_webView.get(), &QWebEngineView::urlChanged, [this](const QUrl& url) {
        qDebug() << url;
        if (_webView)
        {
            if (url.toString().toStdString().find(LOGIN_REDIRECT_URL.to_std_string()) != std::string::npos)
            {
                _webView->load(QUrl(REGISTER_SERIAL_URL.data()));

            }
            else if (url.toString().toStdString().find(REGISTER_SERIAL_DONE_URL.to_std_string()) != std::string::npos)
            {
                //login success
                //auto cookieStore = _webView->page()->profile()->cookieStore();
                //_webView->close();
                acquireAuth();
                //_webView.reset();
            }
        }

        });
    _webView->load(QUrl(LOGIN_URL.data()));
	_webView->show();

}

//void Hix::Auth::AuthManager::logout()
//{
//	//block usage until authorized
//	blockApp();
//}  


void Hix::Auth::AuthManager::blockApp()
{
    _mainWindow->setProperty("visible", false);

}

void Hix::Auth::AuthManager::unblockApp()
{
    _mainWindow->setProperty("visible", true);
}

void Hix::Auth::AuthManager::clearSavedCks()
{
    auto& moddableSetting = Hix::Application::SettingsChanger::settings(Hix::Application::ApplicationManager::getInstance()).additionalSetting;
    moddableSetting.keyVals.clear();
}


//#define MY_ENCODING_TYPE  (PKCS_7_ASN_ENCODING | X509_ASN_ENCODING)
//
//std::string Hix::Auth::AuthManager::guid() const
//{
//    std::string ret;
//    char value[64];
//    DWORD size = _countof(value);
//    DWORD type = REG_SZ;
//    HKEY key;
//    LONG retKey = ::RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Cryptography", 0, KEY_READ | KEY_WOW64_64KEY, &key);
//    LONG retVal = ::RegQueryValueExA(key, "MachineGuid", nullptr, &type, (LPBYTE)value, &size);
//    if (retKey == ERROR_SUCCESS && retVal == ERROR_SUCCESS) {
//        ret = value;
//    }
//    ::RegCloseKey(key);
//    return ret;
//}
//
//

//std::string encryptStr(const std::string& value)
//{
//    DATA_BLOB DataIn;
//    DATA_BLOB DataOut;
//    BYTE* pbDataInput = (BYTE*)value.c_str();
//    DWORD cbDataInput = value.length() + 1;
//    DataIn.pbData = pbDataInput;
//    DataIn.cbData = cbDataInput;
//    CRYPTPROTECT_PROMPTSTRUCT PromptStruct;
//    //LPWSTR pDescrOut = NULL;
//
//    //-------------------------------------------------------------------
//    //  Initialize PromptStruct.
//
//    ZeroMemory(&PromptStruct, sizeof(PromptStruct));
//    PromptStruct.cbSize = sizeof(PromptStruct);
//    //PromptStruct.dwPromptFlags = CRYPTPROTECT_PROMPT_ON_PROTECT;
//    //PromptStruct.szPrompt = L"This is a user prompt.";
//    if (!CryptProtectData(
//        &DataIn,
//        nullptr, // A description string. 
//        NULL,                               // Optional entropy
//                                            // not used.
//        NULL,                               // Reserved.
//        &PromptStruct,                      // Pass a PromptStruct.
//        0,
//        &DataOut))
//    {
//        throw std::runtime_error("Encryption error!");
//    }
//
//    std::string returnVal((char*)DataOut.pbData, DataOut.cbData);
//    LocalFree(DataOut.pbData);
//    return returnVal;
//}
//
//
//std::string decryptStr(const std::string& value)
//{
//    DATA_BLOB encrypted;
//    DATA_BLOB decrypted;
//    BYTE* pbDataInput = (BYTE*)value.c_str();
//    DWORD cbDataInput = value.length() + 1;
//    encrypted.pbData = pbDataInput;
//    encrypted.cbData = cbDataInput;
//    CRYPTPROTECT_PROMPTSTRUCT PromptStruct;
//    ZeroMemory(&PromptStruct, sizeof(PromptStruct));
//    PromptStruct.cbSize = sizeof(PromptStruct);
//    //PromptStruct.dwPromptFlags = CRYPTPROTECT_PROMPT_ON_PROTECT;
//    //PromptStruct.szPrompt = L"This is a user prompt.";
//    if (!CryptUnprotectData(
//        &encrypted,
//        nullptr,              // desc not used
//        NULL,                 // Optional entropy
//        NULL,                 // Reserved
//        &PromptStruct,        // Optional PromptStruct
//        0,
//        &decrypted))
//    {
//        throw std::runtime_error("Decryption error!");
//    }
//    std::string returnVal((char*)decrypted.pbData, decrypted.cbData);
//    qDebug() << returnVal.c_str();
//    LocalFree(decrypted.pbData);
//    return returnVal;
//
//
//}
//
//
//
//
//void storeCk(const QNetworkCookie& ck, Hix::Settings::AdditionalSetting& json)
//{
//    json.keyVals[ck.name().toStdString()] = encryptStr(ck.value().toStdString());
//    json.writeJSON();
//}
//
//void loadCk(QWebEngineCookieStore& jar, const Hix::Settings::AdditionalSetting& json)
//{
//    for (auto& ck : json.keyVals)
//    {
//        jar.setCookie(QNetworkCookie(QByteArray::fromStdString(ck.first), QByteArray::fromStdString(decryptStr(ck.second))));
//    }
//}
//
//
//void loadCk(QList<QNetworkCookie>& jar, const Hix::Settings::AdditionalSetting& json)
//{
//    for (auto& ck : json.keyVals)
//    {
//        jar.append(QNetworkCookie(QByteArray::fromStdString(ck.first), QByteArray::fromStdString(decryptStr(ck.second))));
//    }
//}
