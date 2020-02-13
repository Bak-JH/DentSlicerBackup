#include "FlushSupport.h"
#include "qmlmanager.h"
#include "feature/SupportFeature.h"




Hix::Features::FlushSupport::FlushSupport(std::unordered_set<GLModel*>& models)
{
	auto flshSupp = qmlManager->supportRaftManager().modelAttachedSupports(models);
	if (!flshSupp.empty())
	{
		qmlManager->taskManager().enqueTask(SupportMode().clearSupport(models));
	}
}

Hix::Features::FlushSupport::FlushSupport(GLModel* model) : FlushSupport({model})
{
}

Hix::Features::FlushSupport::~FlushSupport()
{
}

Hix::Features::FeatureContainerFlushSupport::FeatureContainerFlushSupport(std::unordered_set<GLModel*>& models) : FlushSupport(models)
{
}

Hix::Features::FeatureContainerFlushSupport::FeatureContainerFlushSupport(GLModel* model): FlushSupport(model)
{
}

Hix::Features::FeatureContainerFlushSupport::~FeatureContainerFlushSupport()
{
}
