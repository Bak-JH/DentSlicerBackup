#include "FlushSupport.h"
#include "qmlmanager.h"
#include "feature/SupportFeature.h"

Hix::Features::FlushSupport::FlushSupport()
{
	if(!qmlManager->supportRaftManager().supportsEmpty())
		qmlManager->featureHistoryManager().addFeature(SupportMode(qmlManager->getSelectedModels(), qmlManager->models).clearSupport());
}

Hix::Features::FlushSupport::~FlushSupport()
{
}
