#include "RaftModel.h"
#include "SupportRaftManager.h"
#include "../glmodel.h"
Hix::Support::RaftModel::RaftModel(SupportRaftManager* manager): Hix::Render::SceneEntityWithMaterial(&manager->rootEntity()), _manager(manager)
{
	setMaterialColor(Hix::Render::Colors::Raft);
}

Hix::Support::RaftModel::~RaftModel()
{

}
