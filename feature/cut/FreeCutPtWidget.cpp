#include "FreeCutPtWidget.h"
#include "DrawingPlane.h"
#include "../../application/ApplicationManager.h"

#include "render/LineMeshEntity.h"

using namespace Hix::Features::Cut;
using namespace Hix::Application;
#if defined(_DEBUG) || defined(QT_DEBUG)
#define _STRICT_FREECUT
#endif

FreeCutPtWidget::FreeCutPtWidget(DrawingPlane* drawingPlane):Hix::UI::CubeWidget(drawingPlane), _drawingPlane(drawingPlane)
{
	//_transform.setScale(0.5f);
	_material.setAmbient(QColor(0,0,0));
	_material.setDiffuse(QColor(0, 0, 0));
	_material.setSpecular(QColor(0, 0, 0));
	setHitTestable(true);
	setHoverable(true);

}
FreeCutPtWidget::~FreeCutPtWidget()
{
}
void FreeCutPtWidget::onEntered()
{
	Hix::Application::ApplicationManager::getInstance().cursorManager().setCursor(CursorType::Eraser);
}

void FreeCutPtWidget::onExited()
{
	Hix::Application::ApplicationManager::getInstance().cursorManager().setCursor(CursorType::Default);
}

void FreeCutPtWidget::clicked(Hix::Input::MouseEventData& e, const Qt3DRender::QRayCasterHit& hit)
{
	Hix::Application::ApplicationManager::getInstance().cursorManager().setCursor(CursorType::Default);
	_drawingPlane->removePt(this);
	//don't do anything for obvious reasons.
}

void Hix::Features::Cut::FreeCutPtWidget::updateLineTo()
{
	if (prev != nullptr)
	{
		std::vector<QVector3D> path;
		path.emplace_back(prev->translation() - translation());
		path.emplace_back(QVector3D(0,0,0));
		_line.reset(new Hix::Render::LineMeshEntity(path, this));
	}
	else
	{
		_line.reset();
	}

}

void Hix::Features::Cut::FreeCutPtWidget::setTranslation(const QVector3D& trans)
{
	_transform.setTranslation(trans);
	updateLineTo();
}

QVector3D Hix::Features::Cut::FreeCutPtWidget::translation() const
{
	return _transform.translation();
}
