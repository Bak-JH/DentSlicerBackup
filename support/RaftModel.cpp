#include "RaftModel.h"
#include "SupportRaftManager.h"
#include "../glmodel.h"
Hix::Support::RaftModel::RaftModel(SupportRaftManager* manager): Hix::Render::SceneEntityWithMaterial(manager->getModel()), _manager(manager)
{
	_meshMaterial.setColor(Hix::Render::Colors::Raft);
}

Hix::Support::RaftModel::~RaftModel()
{

}
