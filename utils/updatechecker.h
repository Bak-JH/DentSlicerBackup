#pragma once
#include "winsparkle/winsparkle.h"
#include <QResource>
#include <QNetworkReply>
#include <QXmlStreamReader>

namespace Hix
{
    namespace Utils
    {
        class UpdateChecker : public QObject
        {
            Q_OBJECT
        public:
            UpdateChecker();
            void init();
            void checkForUpdates();
            void forceCheckForUpdates();
        public slots:
            void parseUpdateInfo(QNetworkReply*);

        private:
            void initWinSparkle();

            std::unique_ptr<QNetworkAccessManager> _manager;
        };


    }
}
