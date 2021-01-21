


#include <QString>
#include <QtCore/QObject>

namespace Qt3DCore
{
    class QEntity;
}


class Lights: public QObject
{
    Q_OBJECT

public:
    explicit Lights(Qt3DCore::QEntity *rootEntity);
private:
    Qt3DCore::QEntity *parentEntity;
};
