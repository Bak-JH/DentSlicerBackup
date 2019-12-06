#include "FeatureHistoryManager.h"

void Hix::Features::FeatureHisroyManager::addFeature(Hix::Features::Feature* feature)
{
	if (feature == nullptr)
		return;

	_history.emplace_back(std::unique_ptr<Feature>(feature));
}

void Hix::Features::FeatureHisroyManager::undo()
{
	if (_history.empty())
		return;

	auto prevFeature = _history.back().release();
	if (prevFeature)
		prevFeature->undo();
	_history.pop_back();
}
