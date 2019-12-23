#include "Feature.h"
#include "qmlmanager.h"
#include <QDebug>
using namespace Hix::Features;

Hix::Features::Feature::Feature()
{
}


Hix::Features::Feature::~Feature()
{
}

void Hix::Features::Feature::undo()
{
	qmlManager->unselectAll();
	undoImpl();
}

void Hix::Features::Feature::redo()
{
	qmlManager->unselectAll();
	redoImpl();
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

void Hix::Features::FeatureContainer::undoImpl()
{

	for (auto each = _container.rbegin(); each != _container.rend(); ++each)
	{
		each->get()->undo();
	}
}

void Hix::Features::FeatureContainer::redoImpl()
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
