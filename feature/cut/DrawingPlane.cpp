#include "DrawingPlane.h"
#include "FreeCutPtWidget.h"

using namespace Hix::Input;
using namespace Hix::Features::Cut;

DrawingPlane::DrawingPlane(Qt3DCore::QEntity* owner): Hix::Render::PlaneMeshEntity(owner)
{
	initHitTest();
	setEnabled(false);
	setHitTestable(false);
}

Hix::Features::Cut::DrawingPlane::~DrawingPlane()
{
}

void DrawingPlane::enablePlane(bool isEnable)
{
	if (isEnable != isEnabled())
	{
		setEnabled(isEnable);
		if (!isEnable)
		{
			enableDrawing(false);
		}
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
	pos.setZ(_transform.translation().z());
	auto emplResult = _ptWidgets.emplace(Hix::Memory::toUnique(new FreeCutPtWidget(this)));
	auto newLatest = emplResult.first->get();
	newLatest->prev = _lastPt;
	if (_lastPt)
	{
		_lastPt->next = newLatest;
	}
	_lastPt = newLatest;
	newLatest->setTranslation(pos);
	if (_ptWidgets.size() >= 2)
	{
		//QMetaObject::invokeMethod(Hix::Application::ApplicationManager::getInstance().cutPopup, "colorApplyFinishButton", Q_ARG(QVariant, 2));
	}

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
	//kinda hacky, create a tmp unique_ptr for the sake of hashing/look up
	_ptWidgets.erase(Hix::Memory::toDummy(pt));
	//don't double delete
	
	if (_ptWidgets.size() < 2)
	{
		//QMetaObject::invokeMethod(Hix::Application::ApplicationManager::getInstance().cutPopup, "colorApplyFinishButton", Q_ARG(QVariant, 0));
	}

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
		auto beginOfPath = _ptWidgets.begin();
		for (; beginOfPath != _ptWidgets.end(); ++beginOfPath)
		{
			if ((*beginOfPath)->prev == nullptr)
			{
				break;
			}
		}
		FreeCutPtWidget* curr = &**beginOfPath;
		while (curr != nullptr)
		{
			path.emplace_back(curr->translation());
			curr = curr->next;
		}
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