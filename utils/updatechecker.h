#ifndef UPDATECHECKER_H
#define UPDATECHECKER_H
#include "winsparkle/winsparkle.h"
#include <QResource>
#include <QNetworkReply>
#include <QXmlStreamReader>

class UpdateChecker : public QObject
{
    Q_OBJECT
    QNetworkAccessManager *manager;
    QString current_version;
    QString recent_version;

public:
    UpdateChecker();
    void initWinSparkle();
    void checkForUpdates();

public slots:
    void parseUpdateInfo(QNetworkReply*);
};

#endif // UPDATECHECKER_H
