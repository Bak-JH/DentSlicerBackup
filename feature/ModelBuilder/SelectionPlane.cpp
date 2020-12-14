#include "SelectionPlane.h"
#include "../cut/FreeCutPtWidget.h"
#include "../../application/ApplicationManager.h"

using namespace Hix::Input;
using namespace Hix::Features::Cut;
using namespace Hix::Features;

void Hix::Features::SelectionPlane::clicked(Hix::Input::MouseEventData& m, const Qt3DRender::QRayCasterHit& hit)
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
	newLatest->next = _firstPt;
	if (_ptWidgets.size() == 1)
	{
		_firstPt = newLatest;
	}
	if (_lastPt)
	{
		_lastPt->next = newLatest;
	}
	_lastPt = newLatest;
	newLatest->setTranslation(pos);
	if (_firstPt)
	{
		_firstPt->prev = newLatest;
		_firstPt->updateLineTo();
	}

}

void Hix::Features::SelectionPlane::removePt(FreeCutPtWidget* pt)
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
	if (_firstPt == pt)
	{
		_firstPt = pt->next;
	}
	if (_ptWidgets.size() == 1)
	{
		_firstPt = nullptr;
		_lastPt = nullptr;
	}
	//kinda hacky, create a tmp unique_ptr for the sake of hashing/look up
	_ptWidgets.erase(Hix::Memory::toDummy(pt));
}

void Hix::Features::SelectionPlane::clearPt()
{
	_ptWidgets.clear();
	_lastPt = nullptr;
	_firstPt = nullptr;
}
