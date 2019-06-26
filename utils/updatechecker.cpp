#include "updatechecker.h"


UpdateChecker::UpdateChecker()
{
    manager = new QNetworkAccessManager();
    initWinSparkle();
}

void UpdateChecker::initWinSparkle(){
    // Setup updates feed. This must be done before win_sparkle_init(), but
    // could be also, often more conveniently, done using a VERSIONINFO Windows
    // resource. See the "psdk" example and its .rc file for an example of that
    // (these calls wouldn't be needed then).
    win_sparkle_set_appcast_url("http://18.184.77.105/appcast/appcast.xml");
    const wchar_t* version_w = L"1.1.3";
    current_version = QString::fromWCharArray(version_w);
    win_sparkle_set_app_details(L"HiX.org", L"DentSlicerSetup", version_w);

    // Set DSA public key used to verify update's signature.
    // This is na example how to provide it from external source (i.e. from Qt
    // resource). See the "psdk" example and its .rc file for an example how to
    // provide the key using Windows resource.
    win_sparkle_set_dsa_pub_pem(reinterpret_cast<const char *>(QResource(":/Resource/pem/dsa_pub.pem").data()));

    // Initialize the updater and possibly show some UI
    win_sparkle_init();
}

void UpdateChecker::checkForUpdates(){
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(parseUpdateInfo(QNetworkReply*)));
    manager->get(QNetworkRequest(QUrl("http://18.184.77.105/appcast/appcast.xml")));

    //win_sparkle_check_update_with_ui_and_install();
    win_sparkle_check_update_without_ui();
    //win_sparkle_check_update_with_ui();
}

void UpdateChecker::parseUpdateInfo(QNetworkReply* reply){
    QString replyText = reply->readAll();

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
                       recent_version = s.split(" ")[1];
                   }
               }
           }
       }
}
