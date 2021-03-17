#include "DrawingPlane.h"
#include "FreeCutPtWidget.h"

using namespace Hix::Input;
using namespace Hix::Features::Cut;




auto inline defaultWidth()
{
	return  Hix::Application::ApplicationManager::getInstance().settings().printerSetting.bedBound.lengthX();
}

auto inline defaultHeight()
{
	return  Hix::Application::ApplicationManager::getInstance().settings().printerSetting.bedBound.lengthY();
}
const QColor defaultColor(140, 140, 140);
constexpr float defaultAlpha = 0.2f;
constexpr bool defaultDoubleSided = true;

Hix::Features::Cut::DrawingPlane::DrawingPlane(Qt3DCore::QEntity* owner, float width, float height, QColor color, float alpha, bool isDoubleSided):
	Hix::Render::PlaneMeshEntity(owner, width, height, color, isDoubleSided, alpha)
{
	initHitTest();
	setEnabled(false);
	setHitTestable(false);
}
Hix::Features::Cut::DrawingPlane::DrawingPlane(Qt3DCore::QEntity* owner, float alpha): DrawingPlane(owner, defaultWidth(), defaultHeight(), defaultColor, alpha, defaultDoubleSided)
{
}

Hix::Features::Cut::DrawingPlane::DrawingPlane(Qt3DCore::QEntity* owner):DrawingPlane(owner, defaultWidth(), defaultHeight(), defaultColor, defaultAlpha, defaultDoubleSided)
{
}

Hix::Features::Cut::DrawingPlane::~DrawingPlane()
{
}

void DrawingPlane::enablePlane(bool isEnable)
{
	if (isEnable != isEnabled())
	{
		setEnabled(isEnable);
		enableDrawing(isEnable);
	}
}



void DrawingPlane::enableDrawing(bool isEnable)
{
	if (isEnable != _drawingEnabled)
	{
		_drawingEnabled = isEnable;
		setHitTestable(_drawingEnabled);
	}
}


void DrawingPlane::clicked(Hix::Input::MouseEventData& m, const Qt3DRender::QRayCasterHit& hit)
{
	auto hitEntity = hit.entity();
	//since plane has its own transformation, we need to adjust the hit test coordinate accordingly.
	QVector3D pos = hit.localIntersection();
	auto entityTransform = _meshTransformMap[hitEntity];
	pos = entityTransform->matrix() * pos;
	//pos.setZ(_transform.translation().z());
	auto emplResult = _ptWidgets.emplace(Hix::Memory::toUnique(new FreeCutPtWidget(this)));
	auto newLatest = emplResult.first->get();
	newLatest->prev = _lastPt;
	if (_lastPt)
	{
		_lastPt->next = newLatest;
	}
	_lastPt = newLatest;
	newLatest->setTranslation(pos);

}

void Hix::Features::Cut::DrawingPlane::removePt(FreeCutPtWidget* pt)
{
	auto next = pt->next;
	auto prev = pt->prev;
	if (next)
	{
		next->prev = prev;
		next->updateLineTo();
	}
	if (prev)
	{
		prev->next = next;
	}
	//update last pt
	if (_lastPt == pt)
	{
		_lastPt = pt->prev;
	}
	if (_ptWidgets.size() == 1)
	{
		_lastPt = nullptr;
	}
	//kinda hacky, create a tmp unique_ptr for the sake of hashing/look up
	_ptWidgets.erase(Hix::Memory::toDummy(pt));

}


void Hix::Features::Cut::DrawingPlane::clearPt()
{
	_ptWidgets.clear();
	_lastPt = nullptr;

}

std::vector<QVector3D> Hix::Features::Cut::DrawingPlane::contour() const
{
	std::vector<QVector3D> path;
	path.reserve(_ptWidgets.size());
	if (!_ptWidgets.empty())
	{
		auto curr = _lastPt;
		//we use count instead of null check due to possible circular contour
		while (path.size() < _ptWidgets.size())
		{
			path.push_back(curr->translation());
			curr = curr->prev;
		}
		std::reverse(path.begin(), path.end());

	}
	return path;
}

void DrawingPlane::initHitTest()
{
	for (auto& p : _meshTransformMap)
	{
		p.first->addComponent(&_layer);
	}
	_layer.setRecursive(false);

}