#include "Feature.h"
#include "qmlmanager.h"
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
	try
	{
		qmlManager->unselectAll();
		undoImpl();
	}
	catch (std::exception & e)
	{
		qDebug() << e.what();
		//popup, log, send error report
	}
	catch (...)
	{
		qDebug() << "undo failed, unkown exception";
	}
}

void Hix::Features::Feature::redo()noexcept
{
	try
	{
		qmlManager->unselectAll();
		redoImpl();
	}
	catch (std::exception & e)
	{
		qDebug() << e.what();
		//popup, log, send error report
	}
	catch (...)
	{
		qDebug() << "undo failed, unkown exception";
	}
}

void Feature::postUIthread(std::function<void()>&& func)
{
	QMetaObject::invokeMethod(qmlManager, func, Qt::BlockingQueuedConnection);
}
void Hix::Features::Feature::run()noexcept
{

	bool success = false;
	try
	{
		runImpl();
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
		qmlManager->featureHistoryManager().addFeature(this);
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
