#include "updatechecker.h"
#include "Settings/AppSetting.h"
#include "application/ApplicationManager.h"
using namespace Hix::Utils;
constexpr auto APPCAST = "https://services.hix.co.kr/setup/view_file/dentslicer/appcast.xml";
UpdateChecker::UpdateChecker()
{
}

void UpdateChecker::initWinSparkle(){
    // Setup updates feed. This must be done before win_sparkle_init(), but
    // could be also, often more conveniently, done using a VERSIONINFO Windows
    // resource. See the "psdk" example and its .rc file for an example of that
    // (these calls wouldn't be needed then).
    
    auto version = Hix::Application::ApplicationManager::getInstance().getVersion().toStdWString();
    qDebug() << "ver: " << version;

    win_sparkle_set_appcast_url(APPCAST);

    win_sparkle_set_app_details(L"HiX.org", L"DentSlicerSetup", version.c_str());

    // Set DSA public key used to verify update's signature.
    // This is na example how to provide it from external source (i.e. from Qt
    // resource). See the "psdk" example and its .rc file for an example how to
    // provide the key using Windows resource.
    win_sparkle_set_dsa_pub_pem(reinterpret_cast<const char *>(QResource(":/Resource/pem/service_pub.pem").data()));


    // Initialize the updater and possibly show some UI
    win_sparkle_init();
}

void UpdateChecker::init()
{
    _manager.reset(new QNetworkAccessManager());
    QObject::connect(_manager.get(), &QNetworkAccessManager::finished,
        [this](QNetworkReply* reply) {
            parseUpdateInfo(reply);
        });
    initWinSparkle();
}

void UpdateChecker::checkForUpdates(){
    _manager->get(QNetworkRequest(QUrl(APPCAST)));

    //win_sparkle_check_update_with_ui_and_install();
    win_sparkle_check_update_without_ui();
    //win_sparkle_check_update_with_ui();
}

void Hix::Utils::UpdateChecker::forceCheckForUpdates()
{
    _manager->get(QNetworkRequest(QUrl(APPCAST)));
    win_sparkle_check_update_with_ui();
}

void UpdateChecker::parseUpdateInfo(QNetworkReply* reply){
    QString replyText = reply->readAll();
    QString latest_version;
    QXmlStreamReader reader (replyText);
    while (!reader.atEnd())
       {
           reader.readNext();

           if (reader.error())
           {
               qDebug() << "error";
               return;
           } else {
               if (reader.name().toString() == "title"){
                   QString s = reader.readElementText();
                   if (s.contains("Version")){
                       latest_version = s.split(" ")[1];
                   }
               }
           }
       }
}
