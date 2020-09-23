#include "GridBed.h"
#include "../render/CircleMeshEntity.h"
#include "../render/LineMeshEntity.h"
#include "../render/PlaneMeshEntity.h"
#include "../feature/Shapes2D.h"
#include "../application/ApplicationManager.h"
using namespace Hix;
using namespace Hix::Render;
using namespace Hix::Settings;
using namespace Hix::UI;
constexpr float LOGO_MARGIN = 2;
constexpr float CIRCLE_SEG_CNT = 40;
constexpr float GRID_Z_OFFSET = std::numeric_limits<float>::epsilon() * 100;
Hix::UI::GridBed::GridBed(): _logoTransform(new Qt3DCore::QTransform())
{
	_logo.reset(new Qt3DCore::QEntity());
	Qt3DRender::QMesh* mesh = new Qt3DRender::QMesh();
	mesh->setSource(QUrl(QStringLiteral("qrc:/Resource/mesh/plate_logo.stl")));
	auto mat = new Qt3DExtras::QPhongMaterial();
	QColor color(150, 150, 150, 255);
	mat->setAmbient(color);
	mat->setDiffuse(color);
	mat->setSpecular(color);
	_logo->addComponent(mesh);
	_logo->addComponent(mat);
	_logo->addComponent(_logoTransform);
	_logoTransform->setScale(0.5f);
}

Hix::UI::GridBed::~GridBed()
{
	_logo.release();
	_bedShape.release();
	_grid.release();
}
void Hix::UI::GridBed::drawBed()
{
	auto parent = Hix::Application::ApplicationManager::getInstance().sceneManager().total();
	_logo->setParent(parent);
	auto& printerSettings = Hix::Application::ApplicationManager::getInstance().settings().printerSetting;
	switch (printerSettings.bedShape)
	{
	case Hix::Settings::PrinterSetting::BedShape::Circle:
	{
		auto bed = new CircleMeshEntity(printerSettings.bedRadius, CIRCLE_SEG_CNT, Qt::white, parent);
		_bedShape.reset(bed);
		auto circleGrid = Hix::Shapes2D::gridCircle(printerSettings.bedRadius, 5); //5mm
		for (auto& p : circleGrid)
		{
			Hix::Shapes2D::setZ(GRID_Z_OFFSET, p.begin(), p.end());
		}
		_grid.reset(new LineMeshEntity(circleGrid, parent, Qt::black));
		_logoTransform->setTranslation(QVector3D(0, -LOGO_MARGIN -printerSettings.bedRadius, 0));
		break;
	}
	case Hix::Settings::PrinterSetting::BedShape::Rect:
	{
		auto bed = new PlaneMeshEntity(parent, printerSettings.bedX, printerSettings.bedY, Qt::white);
		_bedShape.reset(bed);
		auto rectGrid = Hix::Shapes2D::gridRect(printerSettings.bedX, printerSettings.bedY, 5); //5mm
		for (auto& p : rectGrid)
		{
			Hix::Shapes2D::setZ(GRID_Z_OFFSET, p.begin(), p.end());
		}
		_grid.reset(new LineMeshEntity(rectGrid, parent, Qt::black));
		_logoTransform->setTranslation(QVector3D(0,  -LOGO_MARGIN -printerSettings.bedY/2.0f, 0));
		break;
	}
	default:
		break;
	}
}