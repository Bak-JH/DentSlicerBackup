#include "FeatureHistoryManager.h"
#include <QDebug>

Hix::Features::FeatureHistoryManager::FeatureHistoryManager()
{
	_itr = _history.end();
}

void Hix::Features::FeatureHistoryManager::addFeature(Hix::Features::Feature* feature)
{
	if (feature == nullptr)
	{
		return;
	}
	auto container = dynamic_cast<FeatureContainer*>(feature);
	if (container && container->empty())
	{
		return;
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

const Hix::Features::Feature& Hix::Features::FeatureHistoryManager::peek()const
{
	return *(_history.back().get());
}

bool Hix::Features::FeatureHistoryManager::empty() const
{
	return _history.empty();
}

void Hix::Features::FeatureHistoryManager::pop()
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

void Hix::Features::FeatureHistoryManager::undo()
{
	if (_itr == _history.begin())
		return;
	--_itr;
	auto prevFeature = _itr->get();
	prevFeature->undo();
}

void Hix::Features::FeatureHistoryManager::redo()
{
	if (_itr == _history.end())
		return;
	auto prevFeature = _itr->get();
	prevFeature->redo();
	++_itr;
}