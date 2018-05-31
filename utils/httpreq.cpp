#include "httpreq.h"
#include <stdio.h>
#include <iostream>
#include <QtNetwork>
#include <QThread>
#include <QtConcurrent/QtConcurrent>
#include <QApplication>
#include <QSsl>
#include <QMessageBox>
#include "qmlmanager.h"
#include <QCryptographicHash>

httpreq::httpreq()
{
    main_url = "http://18.184.77.105:8014/";

    manager = new QNetworkAccessManager();

    this->connect(manager, SIGNAL(finished(QNetworkReply*)), SLOT(replyFinished(QNetworkReply*)));

    get_csrf_token();
}

httpreq::~httpreq()
{

}

void httpreq::replyFinished(QNetworkReply *reply)
{
    req_reply = reply->readAll();
    QString encoded = QString::fromUtf8(req_reply);
    qDebug() << "req_reply : " << req_reply;
    if (strstr(req_reply, "authorized")){
        // if serial is not stored in env, save it
        QString serial_path = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
        QDir dir(serial_path );
        if (!dir.exists())
            dir.mkpath(serial_path );
        if (!dir.exists("infos"))
            dir.mkdir("infos");

        QString serial_key = req_reply.split('#')[0].split('=')[1];

        dir.cd("infos");
        serial_path = dir.absoluteFilePath("stored_serial.txt");

        if (QFile::exists(serial_path)){
            // get serial
            QFile file(serial_path);
            if (file.open(QIODevice::ReadWrite)){
                QTextStream stream(&file);
                stored_serial = stream.readLine();
            }
            qDebug() << "stored_serial : " << stored_serial.mid(0,16);

            // create serial if serial key doesn't match stored_serial
            if (stored_serial.mid(0,16) !=serial_key.mid(0,16)){
                QFile file(serial_path);
                if (file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text)){
                    QTextStream stream(&file);
                    qDebug() << "rewriting serial : " << serial_key;
                    stream << serial_key;
                }
                file.close();
            }
        } else {
            QFile file(serial_path);
            if (file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text)){
                QTextStream stream(&file);
                qDebug() << "serial : " << serial_key;
                stream << serial_key;
            }
            file.close();
        }

        qDebug() << "authorized";

        QMetaObject::invokeMethod(qmlManager->loginButton, "loginSuccess");
    } else if (strstr(req_reply, "csrftoken")){ // sets session cookie and csrf token
        QVariant cookieVar = reply->header(QNetworkRequest::SetCookieHeader);
        if (cookieVar.isValid()) {
            QList<QNetworkCookie> cookies = cookieVar.value<QList<QNetworkCookie> >();
            foreach (QNetworkCookie cookie, cookies) {
                if (strstr(cookie.name(), "csrftoken")){
                    csrftoken = cookie.value();
                } else if (strstr(cookie.name(), "sessionid")){
                    sessionid = cookie.value();
                }
            }
            if (first_login){
                check_stored_serial();
                first_login = false;
            }
        } else {
            //QMessageBox::information(w, "암호화 에러", "csrftoken을 받아오지 못했습니다.");
        }

    } else if (strstr(req_reply, "None:violation")){
      // req failure
        //QMessageBox::information(w, "암호화 에러", "서버에서 요청을 거부했습니다.");
        qDebug() << "violation occured";

        QMetaObject::invokeMethod(qmlManager->loginButton, "loginFail");

        // open login
        qmlManager->loginWindow->setProperty("visible",true);
    } else {
        get_csrf_token();
        //QMessageBox::information(w, "암호화 에러", "서버에서 요청을 처리할 수 없습니다.");
        qDebug() << "some error on http_req" << encoded << req_reply;

        QMetaObject::invokeMethod(qmlManager->loginButton, "loginFail");

        // open login
        qmlManager->loginWindow->setProperty("visible",true);
    }

}

void httpreq::get_csrf_token(){
    QNetworkRequest request;
    QNetworkReply *reply = NULL;

    QSslConfiguration config = QSslConfiguration::defaultConfiguration();
    config.setProtocol(QSsl::TlsV1_2OrLater);

    request.setSslConfiguration(config);
    request.setUrl(QUrl(main_url+"csrf_generation/"));
    reply = manager->get(request);
}

void httpreq::login(QString id, QString pw){
    QNetworkRequest request;
    QNetworkReply *reply = NULL;

    QSslConfiguration config = QSslConfiguration::defaultConfiguration();
    config.setProtocol(QSsl::TlsV1_2OrLater);

    request.setSslConfiguration(config);
    request.setUrl(QUrl(main_url+"admin_login/"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QString cookiestring ="csrftoken="+csrftoken+";" + "sessionid="+sessionid+";";

    request.setRawHeader((QByteArray) "Cookie", cookiestring.toUtf8());
    request.setRawHeader("Referer", "https://twinsmilelibrary.co.kr");

    QUrlQuery qu;
    qu.addQueryItem("id", id);
    qu.addQueryItem("pw", pw);
    qu.addQueryItem("program", "true");
    qu.addQueryItem("csrfmiddlewaretoken", csrftoken);

    QUrl params;
    params.setQuery(qu);

    reply = manager->post(request, params.toEncoded().split('?')[1]);
}

void httpreq::post_key_info(QString id, QString pw, QString usage, QString admin, QString phone, QString email){
    QNetworkRequest request;
    QNetworkReply *reply = NULL;
    QSslConfiguration config = QSslConfiguration::defaultConfiguration();
    config.setProtocol(QSsl::TlsV1_2OrLater);

    request.setSslConfiguration(config);
    request.setUrl(QUrl(main_url+"register_serial/"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QString cookiestring ="csrftoken="+csrftoken+";" + "sessionid="+sessionid+";";

    request.setRawHeader((QByteArray) "Cookie", cookiestring.toUtf8());
    request.setRawHeader("Referer", "https://twinsmilelibrary.co.kr");

    QUrlQuery qu;
    qu.addQueryItem("userid", id);
    qu.addQueryItem("passwd", pw);
    qu.addQueryItem("usage", usage);
    qu.addQueryItem("admin", admin);
    qu.addQueryItem("phone", phone);
    qu.addQueryItem("email", email);
    qu.addQueryItem("program", "true");
    qu.addQueryItem("csrfmiddlewaretoken", csrftoken);

    QUrl params;
    params.setQuery(qu);

    reply = manager->post(request, params.toEncoded().split('?')[1]);
}


void httpreq::get_uuid(){

    QString uuid_path = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    QDir dir(uuid_path );
    if (!dir.exists())
        dir.mkpath(uuid_path );
    if (!dir.exists("infos"))
        dir.mkdir("infos");

    dir.cd("infos");
    uuid_path = dir.absoluteFilePath("uuid.txt");

    if (QFile::exists(uuid_path )){
        // get uuid
        QFile file(uuid_path);
        if (file.open(QIODevice::ReadWrite)){
            QTextStream stream(&file);
            uuid = stream.readLine();
        }
        qDebug() << "uuid : " << uuid;
    }
    else {
        // create uuid
        QFile file(uuid_path);
        if (file.open(QIODevice::ReadWrite)){
            QTextStream stream(&file);
            extern QUuid quuid;
            uuid = quuid.createUuid().toString();
            uuid.replace("{", "");
            uuid.replace("}", "");
            uuid.replace("-", "");
            qDebug() << "uuid : " << uuid;
            stream << uuid;
        }
    }
}

QString httpreq::get_hash(){

    get_uuid();

    QString name = qgetenv("USER");
    if (name.isEmpty())
        name = qgetenv("USERNAME");

    //SHA256 hash;
    string message = uuid.toStdString() + name.toStdString();
    QByteArray byteArray(message.c_str(), message.length());
    //string digest;

    QString hash = QCryptographicHash::hash(byteArray, QCryptographicHash::Sha256).toHex();
    qDebug () << "hash : " << hash;
    /*StringSource s(message, true, new HashFilter(hash, new HexEncoder(new StringSink(digest))));

    return QString::fromStdString(digest);*/
    return hash;
}

void httpreq::get_iv(QString key){
    QNetworkRequest request;
    QNetworkReply *reply = NULL;
    QSslConfiguration config = QSslConfiguration::defaultConfiguration();
    config.setProtocol(QSsl::TlsV1_2OrLater);

    request.setSslConfiguration(config);

    request.setUrl(QUrl(main_url+"authenticate_serial/"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QString cookiestring ="csrftoken="+csrftoken+";" + "sessionid="+sessionid+";";

    request.setRawHeader((QByteArray) "Cookie", cookiestring.toUtf8());
    request.setRawHeader("Referer", "http://18.184.77.105");

    QString uuid = get_hash().mid(0,32);
    qDebug()<< uuid;
    QString ip = "";
    foreach (const QHostAddress &address, QNetworkInterface::allAddresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress(QHostAddress::LocalHost))
             ip = address.toString();
    }
    qDebug() << ip;

    QUrlQuery qu;
    qu.addQueryItem("serial", key);
    qu.addQueryItem("uuid", uuid);
    qu.addQueryItem("ip", ip);
    qu.addQueryItem("csrfmiddlewaretoken", csrftoken);

    QUrl params;
    params.setQuery(qu);

    reply = manager->post(request, params.toEncoded().split('?')[1]);
}

void httpreq::check_stored_serial(){
    QString serial_path = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    QDir dir(serial_path );
    if (!dir.exists())
        dir.mkpath(serial_path );
    if (!dir.exists("infos"))
        dir.mkdir("infos");

    dir.cd("infos");
    serial_path = dir.absoluteFilePath("stored_serial.txt");

    if (QFile::exists(serial_path)){
        // get serial
        QFile file(serial_path);
        if (file.open(QIODevice::ReadWrite)){
            QTextStream stream(&file);
            stored_serial = stream.readLine();
        }
        qDebug() << "stored_serial : " << stored_serial.mid(0,16);
        get_iv(stored_serial.mid(0,16));
    }
    else {
        // open login
        qmlManager->loginWindow->setProperty("visible",true);
    }

}
