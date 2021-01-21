#include "QMLUtil.h"
#include <deque>
#include <QObject.h>
#include <qstring.h>
#include <qdebug.h>
#include <QQmlContext>
#include <qqml.h>
#include <stdexcept> 

QObject* Hix::QML::findChildQItemByName(QObject* start, const std::string& name)
{
	std::deque<QObject*> s;
	s.push_back(start);
	QString str = QString::fromStdString(name);
	while (!s.empty())
	{
		auto curr = s.front();
		s.pop_front();
		if (curr->objectName() == str)
		{
			return curr;
		}
		auto neighbors = curr->children();
		for (auto& each : neighbors)
		{
			s.push_back(each);
		}
	}
	throw std::runtime_error("failed to find QML element by name" + name);
}

QObject* Hix::QML::findChildQItemByID(QObject* start, const std::string& id)
{
	qDebug() << "start";
	std::deque<QObject*> s;
	s.push_back(start);
	QString str = QString::fromStdString(id);
	while (!s.empty())
	{
		auto curr = s.front();
		s.pop_front();
		auto context = qmlContext(curr);
		if (context && context->nameForObject(curr) == str)
		{
			auto nameInContext = context->nameForObject(curr);
			qDebug() << nameInContext;
			return curr;
		}
		auto neighbors = curr->children();
		for (auto& each : neighbors)
		{
			s.push_back(each);
		}
	}
	throw std::runtime_error("failed to find QML element by id" + id);

}

QObject* Hix::QML::findParendQItemByName(QObject* start, const std::string& name)
{
	auto parent = start->parent();
	QString str = QString::fromStdString(name);
	while (parent != nullptr)
	{
		if (parent->objectName() == str)
		{
			return parent;
		}
		parent = parent->parent();
	}
	throw std::runtime_error("failed to find QML element by name" + name);
}

QObject* Hix::QML::findParendQItemByID(QObject* start, const std::string& id)
{
	auto parent = start->parent();
	QString str = QString::fromStdString(id);
	while (parent != nullptr)
	{
		auto context = qmlContext(parent);
		if (context && context->nameForObject(parent) == str)
		{
			return parent;
		}
		parent = parent->parent();
	}
	throw std::runtime_error("failed to find QML element by id" + id);

}
