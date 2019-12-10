#include "FeatureHistoryManager.h"
#include <QDebug>

Hix::Features::FeatureHisroyManager::FeatureHisroyManager()
{
	_itr = _history.end();
}

void Hix::Features::FeatureHisroyManager::addFeature(Hix::Features::Feature* feature)
{
	if (_history.end() != _itr)
	{
		_history.erase(_itr, _history.end());
	}
	_history.emplace_back(std::unique_ptr<Feature>(feature));
	_itr = _history.end();

}

void Hix::Features::FeatureHisroyManager::undo()
{
	if (_itr == _history.begin())
		return;
	--_itr;
	auto prevFeature = _itr->get();
	prevFeature->undo();
}

void Hix::Features::FeatureHisroyManager::redo()
{
	if (_itr == _history.end())
		return;
	auto prevFeature = _itr->get();
	prevFeature->redo();
	++_itr;
}