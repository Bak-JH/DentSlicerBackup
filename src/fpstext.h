#ifndef FPSTEXT_H
#define FPSTEXT_H

#include <QObject>

class FPSText : public QObject
{
    Q_OBJECT
public:
    explicit FPSText(QObject *parent = nullptr);

signals:

public slots:
};

#endif // FPSTEXT_H