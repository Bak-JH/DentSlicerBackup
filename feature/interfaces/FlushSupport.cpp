#include "FlushSupport.h"
#include "qmlmanager.h"
#include "feature/SupportFeature.h"

Hix::Features::FlushSupport::FlushSupport()
{
	auto selectedModels = qmlManager->getSelectedModels();
	auto flshSupp = qmlManager->supportRaftManager().modelAttachedSupports(selectedModels);
	if (!flshSupp.empty())
	{
		qmlManager->taskManager().enqueTask(SupportMode().clearSupport());
	}
}

//Hix::Features::FlushSupport::FlushSupport(GLModel* model)
//{
//	std::unordered_set<GLModel*> selectedModels;
//	selectedModels.insert(model);
//	auto flshSupp = qmlManager->supportRaftManager().modelAttachedSupports(selectedModels);
//	if (!flshSupp.empty())
//	{
//		qmlManager->taskManager().enqueTask(SupportMode(selectedModels).clearSupport());
//	}
//}

Hix::Features::FlushSupport::~FlushSupport()
{
}
