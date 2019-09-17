#include "SupportModel.h"
#include "SupportRaftManager.h"	
#include "../render/Color.h"
#include "glmodel.h"

using namespace Hix::Engine3D;
using namespace Hix::Support;



Hix::Support::SupportModel::SupportModel(SupportRaftManager* manager, std::variant<VertexConstItr, FaceOverhang> overhang):
	Hix::Render::SceneEntityWithMaterial(manager->getModel()), _manager(manager), _overhang(overhang)
{
	initHitTest();
	setHighlight(false);
}

Hix::Support::SupportModel::~SupportModel()
{
}

void Hix::Support::SupportModel::initHitTest()
{
	addComponent(&_layer);
	_layer.setRecursive(false);

}

void Hix::Support::SupportModel::clicked(Hix::Input::MouseEventData&, const Qt3DRender::QRayCasterHit&)
{
	if (isEnabled())
	{
		switch (_manager->supportEditMode())
		{
		case EditMode::Manual:
			_manager->removeSupport(this);
			break;
		default:
			break;
		}
	}
}



void SupportModel::setHighlight(bool enable)
{
	auto color = Hix::Render::Colors::Support;
	if (enable)
	{
		color = Hix::Render::Colors::SupportHighlighted;
	}
	_meshMaterial.setColor(color);
}

const std::variant<VertexConstItr, Hix::OverhangDetect::FaceOverhang>& Hix::Support::SupportModel::overhang()
{
	return _overhang;
}





void SupportModel::onEntered()
{
	setHighlight(true);
}

void SupportModel::onExited()
{
	setHighlight(false);
}
