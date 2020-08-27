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
using namespace Hix::Features;
using namespace Hix::Engine3D;
namespace bp = boost::process; //we will assume this for all further examples

constexpr auto HTTP = "http://";
constexpr auto PORT = ":5000/";
const QUrl SUPPORT_POPUP_URL = QUrl("qrc:/Qml/FeaturePopup/PopupPrinter.qml");


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
    //boost::asio::io_context ioctx;
    //bp::async_system(ioctx, _exitHandler, "bonjour\mDNSResponder.exe -server");
    //ioctx.run_one();
    //QProcess* process = new QProcess();
    //QString program("bonjour\mDNSResponder.exe");
    //auto result = process->startDetached(program, QStringList() << "-server");
    //auto err0 = process->error();
    //auto err = process->errorString();

    //QProcess* process = new QProcess();

    //QString agentName = "/bonjour/mDNSResponder.exe -server";
    //QString agentPath = QCoreApplication::applicationDirPath() + agentName;
    //QStringList args = QStringList();
    //args = QStringList({ "-Command", QString("Start-Process %1 -Verb runAs").arg(agentPath) });
    //process->start("powershell", args);
    //auto err0 = process->error();
    //auto err = process->errorString();

    auto& co = controlOwner();
    co.getControl(_printersDrop, "printerDrop");
    co.getControl(_refreshButton, "refreshButton");
    // bind buttons
    QObject::connect(_refreshButton, &Hix::QML::Controls::Button::clicked, [this]() {
        refresh();
        });
    QObject::connect(_printersDrop, &Hix::QML::Controls::DropdownBox::indexChanged, [this]() {
        });

    _bonjourBrowser.reset(new BonjourServiceBrowser());
    _bonjourResolver.reset(new BonjourServiceResolver(nullptr));
    _bonjourBrowser->setInterval(100);

    _manager.reset(new QNetworkAccessManager());
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
        QNetworkRequest request(QUrl(HTTP + ip + PORT));
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
        _manager->get(request);
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
    auto test = _mdnsService.running();
    refresh();
    
}


Hix::Features::PrinterServer::~PrinterServer()
{
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


