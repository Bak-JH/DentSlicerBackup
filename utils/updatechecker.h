#ifndef UPDATECHECKER_H
#define UPDATECHECKER_H
#include "winsparkle/winsparkle.h"
#include <QResource>
#include <QNetworkReply>
#include <QXmlStreamReader>

class UpdateChecker : public QObject
{
    Q_OBJECT
public:
    UpdateChecker();
    void setVersion(QString ver);
    void initWinSparkle();
    void checkForUpdates();

public slots:
    void parseUpdateInfo(QNetworkReply*);

private:
    std::unique_ptr<QNetworkAccessManager> _manager;
};

#endif // UPDATECHECKER_H
