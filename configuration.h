#ifndef CONFIGURATION_H
#define CONFIGURATION_H
#include <QVariant>


class Configuration : public QVariantMap
{
public:
    Configuration();
};

extern Configuration* cfg;

#endif // CONFIGURATION_H
