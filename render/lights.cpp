#include "lights.h"
#include <Qt3DCore/qentity.h>
#include <QtMath>
#include <QVector3D>
#include <Qt3DCore/qtransform.h>
#include <Qt3DRender/qdirectionallight.h>
#include <qDebug>

constexpr int light_distance = 100;


Lights::Lights(Qt3DCore::QEntity *rootEntity)
    : parentEntity(rootEntity)
{
	std::vector<QVector3D> lightPos;
	lightPos.push_back(QVector3D(light_distance, light_distance, light_distance));
	lightPos.push_back(QVector3D(light_distance, -light_distance, light_distance));
	lightPos.push_back(QVector3D(-light_distance, -light_distance, -light_distance));
	lightPos.push_back(QVector3D(-light_distance, light_distance, -light_distance));

	//lights shine in center direction
	std::vector<QVector3D> lightDir;
	lightDir.reserve(lightPos.size());
	for (auto& p : lightPos)
	{
		lightDir.push_back(QVector3D(0, 0, 0) - p);
	}


	//light count here is also hardcoded into model_shader.geom!!!
	for (size_t i = 0; i < 4; ++i)
	{
		auto lightEntity = new Qt3DCore::QEntity(parentEntity);
		auto light = new Qt3DRender::QDirectionalLight(lightEntity);
		light->setWorldDirection(lightDir[i]);
		auto lightTransform = new Qt3DCore::QTransform(lightEntity);
		lightTransform->setTranslation(lightPos[i]);
		//assume color is white, intensity 1.0
		//light->setColor("white");
		//light->setIntensity(0.8);
		lightEntity->addComponent(light);
		lightEntity->addComponent(lightTransform);

	}


}


