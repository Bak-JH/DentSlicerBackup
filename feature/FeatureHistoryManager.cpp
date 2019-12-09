#include "FeatureHistoryManager.h"
#include <QDebug>

void Hix::Features::FeatureHisroyManager::addFeature(Hix::Features::Feature* feature)
{
	if (feature == nullptr)
		return;

	_history.emplace_back(std::unique_ptr<Feature>(feature));
	_historyItr = _history.end()-1;
}

void Hix::Features::FeatureHisroyManager::undo()
{
	if (_historyItr == _history.begin())
		return;

	auto prevFeature = _historyItr->get();
	if (prevFeature)
		prevFeature->undo();

	--_historyItr;
}

void Hix::Features::FeatureHisroyManager::redo()
{
	if (_historyItr+1 == _history.end())
		return;

	++_historyItr;
	auto prevFeature = _historyItr->get();
	if (prevFeature)
		prevFeature->redo();
}