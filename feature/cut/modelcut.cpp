#include "modelcut.h"
#include "../../qmlmanager.h"
#include "ZAxialCut.h"
#include "polylinecut.h"
#include "feature/interfaces/Feature.h"
using namespace Hix;
using namespace Hix::Features;
using namespace Hix::Features::Cut;
using namespace Hix::Engine3D;
using namespace Hix::Slicer;
using namespace ClipperLib;



Hix::Features::ModelCut::ModelCut(const std::unordered_set<GLModel*>& selectedModels, Hix::Engine3D::Bounds3D bound):
	_models(selectedModels), _cuttingPlane(qmlManager->total), _modelsBound(bound)
{
	qmlManager->getRayCaster().setHoverEnabled(true);
	_cuttingPlane.enablePlane(true);
}

Hix::Features::ModelCut::~ModelCut()
{
	qmlManager->getRayCaster().setHoverEnabled(false);
}

void ModelCut::cutModeSelected(int type) {
	//if flat cut
	if (type == 1)
	{
		_cutType = ZAxial;
	}
	else if (type == 2)
	{
		_cutType = Polyline;
		_cuttingPlane.enableDrawing(true);
		//want cutting plane to be over model mesh
		float zOverModel = _modelsBound.zMax() + 0.1f;
		_cuttingPlane.transform().setTranslation(QVector3D(0, 0, zOverModel));
		qmlManager->getRayCaster().setHoverEnabled(true);
	}
	return;
}



void ModelCut::getSliderSignal(double value) {
	float zlength = _modelsBound.lengthZ();
	_cuttingPlane.transform().setTranslation(QVector3D(0, 0, _modelsBound.zMin() + value * zlength / 1.8));
}

void Hix::Features::ModelCut::applyCut()
{
	Hix::Features::FeatureContainer* container = new Hix::Features::FeatureContainer();
	switch (_cutType)
	{
	case Hix::Features::ModelCut::ZAxial:
	{
		for (auto each : _models)
		{
			ZAxialCut impl(each, _cuttingPlane.transform().translation().z());
			
		}
		break;
	}
	case Hix::Features::ModelCut::Polyline:
	{
		for (auto each : _models)
		{
			PolylineCut impl(each, _cuttingPlane.contour());
			container->addFeature(impl.getContainer());
		}
		break;
	}
	default:
		break;
	}
	qmlManager->unselectAll();
	qmlManager->featureHistoryManager().addFeature(container);
}