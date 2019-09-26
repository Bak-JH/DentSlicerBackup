#include "lights.h"
#include <qDebug>

Lights::Lights(Qt3DCore::QEntity *rootEntity)
    : parentEntity(rootEntity)
{
	std::vector<QVector3D> lightPos;
	lightPos.push_back(QVector3D(light_distance, light_distance, light_distance));
	lightPos.push_back(QVector3D(light_distance, -light_distance, light_distance));
	lightPos.push_back(QVector3D(-light_distance, -light_distance, -light_distance));
	lightPos.push_back(QVector3D(-light_distance, light_distance, -light_distance));

	//light count here is also hardcoded into model_shader.geom!!!
	for (size_t i = 0; i < 4; ++i)
	{
		auto lightEntity = new Qt3DCore::QEntity(parentEntity);
		auto light = new Qt3DRender::QPointLight(lightEntity);
		auto lightTransform = new Qt3DCore::QTransform(lightEntity);
		lightTransform->setTranslation(lightPos[i]);
		//assume color is white, intensity 1.0
		//light->setColor("white");
		//light->setIntensity(0.8);
		lightEntity->addComponent(light);
		lightEntity->addComponent(lightTransform);

	}


}


