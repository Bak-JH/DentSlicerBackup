#include "VerticalSupportModel.h"
#include <QRenderSettings>
#include <QString>
#include <QtMath>
#include <cfloat>
#include <exception>

#include "qmlmanager.h"
#include "utils/utils.h"
#include <QtConcurrent/QtConcurrent>
#include <QFuture>
#include <QFileDialog>
#include <iostream>
#include <QDir>
#include <QMatrix3x3>
#include <feature/generatesupport.h>
#include "DentEngine/src/configuration.h"
#include "feature/Extrude.h"




using namespace Utils::Math;
using namespace Hix::Engine3D;
using namespace Hix::Input;
using namespace Hix::Render;
using namespace Hix::Support;




VerticalSupportModel::VerticalSupportModel(GLModel* owner, std::variant<VertexConstItr, FaceOverhang> overhang)
    : SceneEntityWithMaterial(owner)

{

	_meshMaterial.changeMode(Hix::Render::ShaderMode::SingleColor);
	_meshMaterial.setColor(Hix::Render::Colors::Default);

	std::vector<QVector3D> contour;
	std::vector<QVector3D> path;
	std::vector<float> scales;

	//contour.push_back(QVector3D(-2, 2, 0));
	//contour.push_back(QVector3D(-2, -2, 0));
	//contour.push_back(QVector3D(2, -2, 0));
	//contour.push_back(QVector3D(2, 2, 0));

	auto a = QVector3D(-2, 2, 0);
	QVector3D normal(0, 0, 1);
	auto rot = QQuaternion::fromAxisAndAngle(normal, 60);
	contour.push_back(a);
	a = rot.rotatedVector(a);
	contour.push_back(a);
	a = rot.rotatedVector(a);
	contour.push_back(a);
	a = rot.rotatedVector(a);
	contour.push_back(a);
	a = rot.rotatedVector(a);
	contour.push_back(a);
	a = rot.rotatedVector(a);
	contour.push_back(a);

	path.push_back(QVector3D(3, 3, 3));
	path.push_back(QVector3D(3, 3, 8));
	path.push_back(QVector3D(3, 3, 10));
	path.push_back(QVector3D(5, 5, 15));
	path.push_back(QVector3D(8, 8, 20));
	scales.push_back(1.0f);
	scales.push_back(2.0f);
	scales.push_back(1.0f);
	scales.push_back(0.5f);
	scales.push_back(0.3f);

	//path.push_back(QVector3D(-2, 2, 0));
	//path.push_back(QVector3D(-2, -2, 0));
	Hix::Features::Extrusion::extrudeAlongPath(_mesh, normal, contour, path, &scales);
	_mesh->addFace(contour[0], contour[1], contour[2]);
	_mesh->addFace(contour[2], contour[3], contour[0]);

	setMesh(_mesh);
	

}

VerticalSupportModel::~VerticalSupportModel()
{

}

void VerticalSupportModel::clicked(Hix::Input::MouseEventData&, const Qt3DRender::QRayCasterHit&)
{

}