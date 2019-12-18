#include "GridBed.h"
#include "../Settings/AppSetting.h"
#include "../render/CircleMeshEntity.h"
#include "../render/LineMeshEntity.h"
#include "../render/PlaneMeshEntity.h"
#include "../feature/Shapes2D.h"
#include "../qmlmanager.h"
using namespace Hix;
using namespace Hix::Render;
using namespace Hix::Settings;
using namespace Hix::UI;

constexpr float CIRCLE_SEG_CNT = 40;
constexpr float GRID_Z_OFFSET = std::numeric_limits<float>::epsilon() * 10;
Hix::UI::GridBed::GridBed()
{
}

Hix::UI::GridBed::~GridBed()
{
}
void Hix::UI::GridBed::drawBed(Qt3DCore::QEntity* parent)
{
	auto& printerSettings = qmlManager->settings().printerSetting();
	switch (printerSettings.bedShape)
	{
	case Hix::Settings::PrinterSetting::BedShape::Circle:
	{
		_bedShape.reset(new CircleMeshEntity(printerSettings.bedRadius, CIRCLE_SEG_CNT, Qt::white, parent));
		auto circleGrid = Hix::Shapes2D::gridCircle(printerSettings.bedRadius, 1); //1mm
		for (auto& p : circleGrid)
		{
			Hix::Shapes2D::setZ(GRID_Z_OFFSET, p.begin(), p.end());
		}
		_grid.reset(new LineMeshEntity(circleGrid, parent, Qt::black));
		break;
	}
	case Hix::Settings::PrinterSetting::BedShape::Rect:
	{
		_bedShape.reset(new PlaneMeshEntity(parent, printerSettings.bedX, printerSettings.bedY, Qt::white));
		auto rectGrid = Hix::Shapes2D::gridRect(printerSettings.bedX, printerSettings.bedY, 1); //1mm
		for (auto& p : rectGrid)
		{
			Hix::Shapes2D::setZ(GRID_Z_OFFSET, p.begin(), p.end());
		}
		_grid.reset(new LineMeshEntity(rectGrid, parent, Qt::black));
		break;
	}
	default:
		break;
	}
}
