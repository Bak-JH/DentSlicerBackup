#include "DrawingPlane.h"
#include "FreeCutPtWidget.h"
#include "../../qmlmanager.h"
using namespace Hix::Input;
using namespace Hix::Features::Cut;

DrawingPlane::DrawingPlane(GLModel* owner): Qt3DCore::QEntity(owner)
{
	auto planeMaterial = new Qt3DExtras::QPhongAlphaMaterial(this);
	planeMaterial->setAmbient(QColor(244,244,244,255));
	planeMaterial->setDiffuse(QColor(244, 244, 244, 255));
	planeMaterial->setSpecular(QColor(244, 244, 244, 255));

	for (int i = 0; i < 1; i++) {
		auto planeEntity = new Qt3DCore::QEntity(this);
		//qDebug() << "generatePlane---------------------==========-=-==-" << parentModel;
		auto clipPlane = new Qt3DExtras::QPlaneMesh(this);
		clipPlane->setHeight(200.0);
		clipPlane->setWidth(200.0);

		auto planeTransform = new Qt3DCore::QTransform();
		planeTransform->setRotation(QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0),  90 + 180 * i));
		planeEntity->addComponent(&_layer);
		planeEntity->addComponent(clipPlane);
		planeEntity->addComponent(planeTransform); //jj
		planeEntity->addComponent(planeMaterial);
		planeEntity->setEnabled(true);

		_meshTransformMap[planeEntity] = planeTransform;
	}
	addComponent(&_transform);



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

Qt3DCore::QTransform& DrawingPlane::transform()
{
	return _transform;
}

void DrawingPlane::clicked(Hix::Input::MouseEventData& m, const Qt3DRender::QRayCasterHit& hit)
{
	auto hitEntity = hit.entity();
	//since plane has its own transformation, we need to adjust the hit test coordinate accordingly.
	QVector3D pos = hit.localIntersection();
	auto entityTransform = _meshTransformMap[hitEntity];
	pos = entityTransform->matrix() * pos;
	pos.setZ(_transform.translation().z());
	auto emplResult = _ptWidgets.emplace(std::make_unique<FreeCutPtWidget>(this));
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
		QMetaObject::invokeMethod(qmlManager->cutPopup, "colorApplyFinishButton", Q_ARG(QVariant, 2));
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
	std::unique_ptr<FreeCutPtWidget> tmp(pt);
	_ptWidgets.erase(tmp);
	//don't double delete
	tmp.release();
	
	if (_ptWidgets.size() < 2)
	{
		QMetaObject::invokeMethod(qmlManager->cutPopup, "colorApplyFinishButton", Q_ARG(QVariant, 0));
	}

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
	addComponent(&_layer);
	_layer.setRecursive(false);

}