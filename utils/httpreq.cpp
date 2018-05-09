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

httpreq::httpreq()
{
    main_url = "http://18.184.77.105:8014/";
    //main_url = "http://hixworld.com:8014/";
    //userid = "dentstudio";
    //passwd = "dentstudio1234!";

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
    qDebug() << req_reply;
    if (strstr(req_reply, "authorized")){
        qDebug() << "authorized";
        QMetaObject::invokeMethod(qmlManager->loginButton, "loginSuccess");


        /*if (strstr(req_reply, "authorized") == 0){
            QMessageBox::information(w, "암호화 에러", "시리얼 키를 받아오지 못했습니다.");
            return; // req failure
        }

        string serial_key = req_reply.split('#')[0].split('=')[1].data();
        string iv = req_reply.split('#')[1].split('=')[1].data();

        bool err;
        string errMsg;
        QString infoldername = w->enc_inpath;
        QString outfoldername = w->enc_outpath;
        list<QString> filelist = findFiles(infoldername);
        list<QString> folderlist = findFolders(infoldername);

        w->reset_progressbar(filelist.size());


        list<QString>::iterator ptr;
        for (ptr = folderlist.begin(); ptr != folderlist.end() ; ptr++){
            QString folderpath = (QString)(*ptr);
            QString foldername = folderpath.mid(infoldername.length(), folderpath.length());
            if (strstr(foldername.toStdString().c_str(), "/."))
                    continue;
            QDir dir(outfoldername+foldername);
            if (!dir.exists()) {
                dir.mkpath(".");
            }
        }


        int cnt = 0;
        for( ptr = filelist.begin() ; ptr != filelist.end() ; ptr++)
        {
            QApplication::processEvents();
            cnt ++;
            QString filepath = (QString)(*ptr);
            QString filename = filepath.mid(infoldername.length(), filepath.length());
            QString encfilename = filename+".enc";

            c->run(filepath.toStdString(), outfoldername.toStdString()+encfilename.toStdString(), serial_key, iv);
        }

        //w->change_error(QString::fromStdString(serial_key).mid(0,16));
        QString serial_split_key;
        for (int i=0; i<4; i++){
            serial_split_key += QString::fromStdString(serial_key).mid(4*i, 4) + "-";
        }
        serial_split_key.remove(serial_split_key.size()-1, 1);
        w->show_key(serial_split_key);
        //w->encryption_finished();
        */
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
        } else {
            //QMessageBox::information(w, "암호화 에러", "csrftoken을 받아오지 못했습니다.");
        }

    } else if (strstr(req_reply, "None:false")){
      // req failure
        //QMessageBox::information(w, "암호화 에러", "서버에서 요청을 거부했습니다.");
        qDebug() << "request failure";
    } else {
        get_csrf_token();
        //QMessageBox::information(w, "암호화 에러", "서버에서 요청을 처리할 수 없습니다.");
        qDebug() << "some error on http_req" << encoded << req_reply;

        QMetaObject::invokeMethod(qmlManager->loginButton, "loginFail");
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
    request.setRawHeader("Referer", "https://twinsmilelibrary.co.kr");

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
