#include "Feature.h"

#include "application/ApplicationManager.h"
#include <QDebug>
using namespace Hix::Features;

Hix::Features::Feature::Feature()
{ 
	//_progress = new Progress();
}


Hix::Features::Feature::~Feature()
{
}

void Hix::Features::Feature::undo()noexcept
{
	//Hix::Application::ApplicationManager::getInstance().partManager().unselectAll();
	undoImpl();
}

void Hix::Features::Feature::redo()noexcept
{
	//Hix::Application::ApplicationManager::getInstance().partManager().unselectAll();
	redoImpl();
}

void Hix::Features::Feature::run()noexcept
{
	runImpl();

	bool success = false;
	try
	{
		success = true;
	}
	catch (std::exception & e)
	{
		qDebug() << e.what();
		//popup, log, send error report
	}
	catch (...)
	{
		//incase of custom unknown exception, gracefully exit
	}
	//if feature add to history
	if (success)
	{
		postUIthread([this]() {
			Hix::Application::ApplicationManager::getInstance().featureManager().featureHistoryManager().addFeature(this);
		});
	}
	else
	{
		delete this;
	}
}


void Hix::Features::Feature::tryRunFeature(Feature& other)
{
	other.runImpl();
}

void Hix::Features::Feature::tryUndoFeature(Feature& other)
{
	other.undoImpl();
}

void Hix::Features::Feature::tryRedoFeature(Feature& other)
{
	other.redoImpl();
}

//void Hix::Features::Feature::tryUndo()
//{
//	redoImpl();
//}
//
//void Hix::Features::Feature::tryRedo()
//{
//	redoImpl();
//}

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
	for (auto& each : _container)
	{
		tryUndoFeature(*each);
	}
}

void Hix::Features::FeatureContainer::redoImpl()
{
	for (auto& each : _container)
	{
		tryRedoFeature(*each);
	}
}

void Hix::Features::FeatureContainer::runImpl()
{
	for (auto& each : _container)
	{
		tryRunFeature(*each);
	}
}

const bool Hix::Features::FeatureContainer::empty()
{
	return _container.empty();
}

std::deque<std::unique_ptr<Feature>>& Hix::Features::FeatureContainer::getContainer()
{
	return _container;
}
