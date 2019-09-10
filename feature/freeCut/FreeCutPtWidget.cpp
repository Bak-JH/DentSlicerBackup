#include "FreeCutPtWidget.h"
#include "DrawingPlane.h"
#include "../../qmlmanager.h"
#include "render/LineMeshEntity.h"

using namespace Hix::Features::Cut;
#if defined(_DEBUG) || defined(QT_DEBUG)
#define _STRICT_FREECUT
#endif

FreeCutPtWidget::FreeCutPtWidget(DrawingPlane* drawingPlane, QVector3D pos):Hix::UI::CubeWidget(drawingPlane), _drawingPlane(drawingPlane)
{
	_transform.setScale(0.5f);
	_material.setAmbient(QColor(0,0,0));
	_material.setDiffuse(QColor(0, 0, 0));
	_material.setSpecular(QColor(0, 0, 0));
	_transform.setTranslation(pos);

}
FreeCutPtWidget::~FreeCutPtWidget()
{
}
void FreeCutPtWidget::onEntered()
{
	qmlManager->setEraserCursor();
}

void FreeCutPtWidget::onExited()
{
	qmlManager->resetCursor();
}

void FreeCutPtWidget::clicked(Hix::Input::MouseEventData& e, const Qt3DRender::QRayCasterHit& hit)
{
	qmlManager->resetCursor();
	_drawingPlane->removePt(this);
	//don't do anything for obvious reasons.
}

void Hix::Features::Cut::FreeCutPtWidget::updateLineTo()
{
	if (prev != nullptr)
	{
		std::vector<QVector3D> path;
		path.emplace_back(prev->translation());
		path.emplace_back(translation());
		_line.reset(new Hix::Render::LineMeshEntity(path, this));
	}

}
