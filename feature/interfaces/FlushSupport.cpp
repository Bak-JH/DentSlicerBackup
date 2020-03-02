#include "FlushSupport.h"
#include "feature/SupportFeature.h"
#include "../../application/ApplicationManager.h"




Hix::Features::FlushSupport::FlushSupport(const std::unordered_set<GLModel*>& models)
{
	auto flshSupp = Hix::Application::ApplicationManager::getInstance().supportRaftManager().modelAttachedSupports(models);
	if (!flshSupp.empty())
	{
		SupportMode().clearSupport(models);
	}
}

Hix::Features::FlushSupport::FlushSupport(GLModel* model)
{
	std::unordered_set<GLModel*> models{ model };
	auto flshSupp = Hix::Application::ApplicationManager::getInstance().supportRaftManager().modelAttachedSupports(models);
	if (!flshSupp.empty())
	{
		SupportMode().clearSupport(models);
	}
}

Hix::Features::FlushSupport::~FlushSupport()
{
}

Hix::Features::FeatureContainerFlushSupport::FeatureContainerFlushSupport(const std::unordered_set<GLModel*>& models) : FlushSupport(models)
{
}

Hix::Features::FeatureContainerFlushSupport::FeatureContainerFlushSupport(GLModel* model): FlushSupport(model)
{
}

Hix::Features::FeatureContainerFlushSupport::~FeatureContainerFlushSupport()
{
}
