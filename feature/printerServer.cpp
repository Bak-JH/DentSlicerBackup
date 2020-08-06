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

#include <QNetworkAccessManager>
#include <QNetworkReply>
using namespace Hix::Features;
using namespace Hix::Engine3D;

constexpr auto HTTP = "http://";
constexpr auto PORT = ":5000/";
const QUrl SUPPORT_POPUP_URL = QUrl("qrc:/Qml/FeaturePopup/PopupPrinter.qml");


Hix::Features::PrinterServer::PrinterServer(): DialogedMode(SUPPORT_POPUP_URL)
{
    auto& co = controlOwner();
    co.getControl(_printersDrop, "printerDrop");
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

    _bonjourBrowser->browseForServiceType(QLatin1String("_C10._tcp"));
    QObject::connect(_manager.get(), &QNetworkAccessManager::finished,
        [this](QNetworkReply* reply) {
            if (reply->error() == QNetworkReply::NoError)
            {
                _printersDrop->appendList(reply->url().toString());
            }
        });

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


