#ifndef HTTPREQ_H
#define HTTPREQ_H
#include <QtNetwork>


class httpreq: public QObject
{
    Q_OBJECT

public:
    QNetworkAccessManager *manager;
    QString main_url;
    QString userid; // need to pass it at compile time
    QString passwd; // need to pass it at compile time
    QString csrftoken;
    QString sessionid;

    QByteArray req_reply;
    httpreq();
    ~httpreq();

    void get_csrf_token();
    void post_key_info(QString id, QString pw, QString usage, QString phone, QString admin, QString email);

public slots:
    void login(QString id, QString pw);
    void replyFinished(QNetworkReply *reply);
};

#endif // HTTPREQ_H
