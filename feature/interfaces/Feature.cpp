#include "Feature.h"
#include <QDebug>
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

	for (auto each = _container.rbegin(); each != _container.rend(); ++each)
	{
		each->get()->undo();
	}
}

void Hix::Features::FeatureContainer::redo()
{
	for (auto& each : _container)
	{
		each->redo();
	}
}

const bool Hix::Features::FeatureContainer::empty()
{
	return _container.empty();
}
