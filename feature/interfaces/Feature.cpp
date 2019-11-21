#include "Feature.h"
using namespace Hix::Features;

Hix::Features::Feature::Feature()
{
}


Hix::Features::Feature::~Feature()
{
}

Hix::Features::FeatureContainer::FeatureContainer()
{
}

void Hix::Features::FeatureContainer::addFeature(Feature* feature)
{
	_container.emplace_back(std::unique_ptr<Feature>(feature));
}

Hix::Features::FeatureContainer::~FeatureContainer()
{
}

void Hix::Features::FeatureContainer::undo()
{
	for (auto& each : _container)
	{
		each->undo();
	}
}
