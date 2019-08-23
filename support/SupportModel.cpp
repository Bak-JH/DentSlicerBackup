#include "SupportModel.h"
#include "SupportRaftManager.h"	
#include "../render/Color.h"


using namespace Hix::Engine3D;
using namespace Hix::Support;



Hix::Support::SupportModel::SupportModel(SupportRaftManager* manager, std::variant<VertexConstItr, FaceOverhang> overhang):
	_manager(manager), _overhang(overhang)
{
	setHighlight(false);
}

Hix::Support::SupportModel::~SupportModel()
{
}

void Hix::Support::SupportModel::clicked(Hix::Input::MouseEventData&, const Qt3DRender::QRayCasterHit&)
{
	switch (_manager->supportEditMode())
	{
	case SupportEditMode::Delete:
		_manager->removeSupport(_overhang);
		break;
	case SupportEditMode::Add:
		break;
	default:
		break;
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





void SupportModel::onEntered()
{
	setHighlight(true);
}

void SupportModel::onExited()
{
	setHighlight(false);
}
