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
	qDebug() << _container.size();
	for (auto& each : _container)
	{
		each->undo();
	}
}

const bool Hix::Features::FeatureContainer::empty()
{
	return _container.empty();
}
