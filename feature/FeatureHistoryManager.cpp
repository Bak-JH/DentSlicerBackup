#include "FeatureHistoryManager.h"
#include <QDebug>

Hix::Features::FeatureHisroyManager::FeatureHisroyManager()
{
	_itr = _history.end();
}

void Hix::Features::FeatureHisroyManager::addFeature(Hix::Features::Feature* feature)
{
	if (feature == nullptr)
	{
		return;
	}

	if (dynamic_cast<FeatureContainer*>(feature))
	{
		if (dynamic_cast<FeatureContainer*>(feature)->empty())
		{
			return;
		}
	}

	if (_history.end() != _itr)
	{
		_history.erase(_itr, _history.end());
	}
	   
	// max size
	if (_history.size() >= 10)
	{
		_history.pop_front();
	}

	_history.emplace_back(std::unique_ptr<Feature>(feature));
	_itr = _history.end();
}

const Hix::Features::Feature& Hix::Features::FeatureHisroyManager::peek()const
{
	return *(_history.back().get());
}

bool Hix::Features::FeatureHisroyManager::empty() const
{
	return _history.empty();
}

void Hix::Features::FeatureHisroyManager::pop()
{
	if (*_itr == _history.back())
	{
		if (_history.size() > 1)
			--_itr;
		else
			_itr = _history.end();
	}
	_history.pop_back();
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