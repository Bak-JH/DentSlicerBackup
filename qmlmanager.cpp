#include "qmlmanager.h"

QmlManager::QmlManager(QObject *parent) : QObject(parent)
{

}



void QmlManager::sendUpdateModelInfo(int printing_time, int layer, QString xyz, float volume){
    updateModelInfo(printing_time, layer, xyz, volume);
}
