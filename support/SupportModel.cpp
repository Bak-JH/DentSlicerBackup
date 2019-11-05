#include "SupportModel.h"
#include "SupportRaftManager.h"	
#include "../render/Color.h"
#include "glmodel.h"

using namespace Hix::Engine3D;
using namespace Hix::Support;



Hix::Support::SupportModel::SupportModel(SupportRaftManager* manager):
	Hix::Render::SceneEntityWithMaterial(&manager->rootEntity()), _manager(manager)
{
	_mesh = new Mesh();
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

void SupportModel::onEntered()
{
	setHighlight(true);
}

void SupportModel::onExited()
{
	setHighlight(false);
}
