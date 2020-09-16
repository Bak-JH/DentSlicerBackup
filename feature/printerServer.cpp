//#define WIN32_LEAN_AND_MEAN
//#include <winsock2.h>
//#include <ws2tcpip.h>
//#include "../common/zeroconf/zeroconf.hpp"
#include "../common/exploreBonjour/bonjourrecord.h"
#include "../common/exploreBonjour/bonjourservicebrowser.h"
#include "../common/exploreBonjour/bonjourserviceresolver.h"

#include "printerServer.h"
#include "../application/ApplicationManager.h"
#include "../Qml/components/Inputs.h"
#include "../Qml/components/Buttons.h"
#include "../settings/LocalSetting.h"


#include <unordered_set>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <algorithm>
#include <QMetaObject>
#include <boost/process.hpp>
#include <iostream>

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <winsqlite/winsqlite3.h>
using namespace Hix::Features;
using namespace Hix::Engine3D;
using namespace Hix::Settings;
using namespace Hix::Settings::JSON;

namespace bp = boost::process; //we will assume this for all further examples

constexpr auto HTTP = "http://";
constexpr auto PORT = ":5000/";
const QUrl SUPPORT_POPUP_URL = QUrl("qrc:/Qml/FeaturePopup/PopupPrinter.qml");


class PrinterServerSetting :public Hix::Settings::LocalSetting
{
public:

    PrinterServerSetting() : Hix::Settings::LocalSetting("printerServer.json")
    {
        parseJSON();
    }
    virtual ~PrinterServerSetting()
    {
        writeJSON();
    }
    std::vector<std::string> savedIPs;

protected:
    void initialize()override
    {
    }
    void parseJSONImpl(const rapidjson::Document& doc)override
    {
        tryParseArray(doc, "savedIPs", savedIPs);
    }
    rapidjson::Document doc()override
    {
        rapidjson::Document doc;
        doc.SetObject();
        rapidjson::Value ipArray(rapidjson::kArrayType);
        for (auto& e : savedIPs)
        {
            rapidjson::Value strVal;
            strVal.SetString(e.c_str(), e.length(), doc.GetAllocator());
            ipArray.PushBack(strVal, doc.GetAllocator());
        }
        doc.AddMember("savedIPs", ipArray, doc.GetAllocator());
        return doc;
    }

};

namespace {
    // declare exit handler function
    void _exitHandler(boost::system::error_code err, int rc) {
        std::cout << "DEBUG async exit error code: "
            << err << " rc: " << rc << std::endl;
    }
}


//cmd /c bonjour\mDNSResponder.exe11 -server
Hix::Features::PrinterServer::PrinterServer(): DialogedMode(SUPPORT_POPUP_URL), _mdnsService("bonjour/mDNSResponder.exe -server")
{
    _manager.reset(new QNetworkAccessManager());
    _printerServerSetting.reset(new PrinterServerSetting());
    auto& co = controlOwner();
    co.getControl(_printersDrop, "printerDrop");
    co.getControl(_refreshButton, "refreshButton");
    // bind buttons
    QObject::connect(_refreshButton, &Hix::QML::Controls::Button::clicked, [this]() {
        refresh();
        });
    QObject::connect(_printersDrop, &Hix::QML::Controls::DropdownBox::indexChanged, [this]() {
        });


    QObject::connect(_manager.get(), &QNetworkAccessManager::finished,
        [this](QNetworkReply* reply) {
            if (reply->error() == QNetworkReply::NoError)
            {
                auto ipStr = reply->url().toString();
                if (!_printersDrop->list().contains(ipStr))
                    _printersDrop->appendList(ipStr);
            }
        });

    //check saved ips
    for (auto& ip : _printerServerSetting->savedIPs)
    {
        checkIP(QString::fromStdString(ip));
    }


    _bonjourBrowser.reset(new BonjourServiceBrowser());
    _bonjourResolver.reset(new BonjourServiceResolver(nullptr));
    _bonjourBrowser->setInterval(100);

    QObject::connect(_bonjourBrowser.get(), &BonjourServiceBrowser::currentBonjourRecordsChanged, [this](const QList<BonjourRecord>& recs) {
        for (auto& e : recs)
        {
            _bonjourResolver->resolveBonjourRecord(e);
        }
        });
    QObject::connect(_bonjourResolver.get(), &BonjourServiceResolver::bonjourRecordResolved, [this](const QHostInfo& info) {
        _bonjourResolver->lookupAddress(info.hostName());
        const QList<QHostAddress>& addresses = info.addresses();
        });

    QObject::connect(_bonjourResolver.get(), &BonjourServiceResolver::foundHostIP, [this](QString ip) {
        checkIP(ip);
        });

    refresh();
}

void Hix::Features::PrinterServer::checkIP(const QString& ip)
{
    QNetworkRequest request(QUrl(HTTP + ip + PORT));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    _manager->get(request);
}
Hix::Features::PrinterServer::~PrinterServer()
{
    //saveIPs
    _printerServerSetting->savedIPs.clear();
    for (auto& e : _printersDrop->list())
    {
        _printerServerSetting->savedIPs.emplace_back(e.toStdString());
    }
}

void Hix::Features::PrinterServer::applyButtonClicked()
{
    if (_printersDrop->size() != 0)
    {
        auto url = _printersDrop->getSelectedItem();
        if (!url.isEmpty())
            QDesktopServices::openUrl(QUrl(url));
    }
}

void Hix::Features::PrinterServer::refresh()
{
    _bonjourBrowser->browseForServiceType(QLatin1String("_C10._tcp"));

}


