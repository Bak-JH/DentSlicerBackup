#include "lights.h"
#include <qDebug>

Lights::Lights(Qt3DCore::QEntity *rootEntity)
    : parentEntity(rootEntity)
{

//Light

    for (int i=0;i<number_of_lights;i++){
    lightEntity[i]= new Qt3DCore::QEntity(parentEntity);
    light[i]=new Qt3DRender::QPointLight(lightEntity[i]);
    light[i]->setColor("white");
    light[i]->setIntensity(0.1);
    lightTransform[i] = new Qt3DCore::QTransform(lightEntity[i]);

    if (i==0)
    lightTransform[i]->setTranslation(QVector3D(light_distance,light_distance,0));
    else if(i==1)
    lightTransform[i]->setTranslation(QVector3D(-light_distance,-light_distance,0));
    else if(i==2)
    lightTransform[i]->setTranslation(QVector3D(-light_distance,light_distance,0));
    else
    lightTransform[i]->setTranslation(QVector3D(light_distance,-light_distance,0));

    lightEntity[i]->addComponent(light[i]);
    lightEntity[i]->addComponent(lightTransform[i]);
    }

}

Lights::~Lights(){
    /*delete[] lightEntity;
    delete[] light;
    delete[] lightTransform;
    delete[] lightEntity;*/
}




