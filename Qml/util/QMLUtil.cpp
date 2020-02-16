#include "QMLUtil.h"
#include <deque>
#include <qquickitem.h>
#include <qstring.h>
#include <qqmlcontext.h>
#include "../../qmlmanager.h"


QQuickItem* Hix::QML::findChildQItemByName(QQuickItem* start, const std::string& name)
{
	std::deque<QQuickItem*> s;
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
		auto neighbors = curr->childItems();
		for (auto& each : neighbors)
		{
			s.push_back(each);
		}
	}
	throw std::runtime_error("failed to find QML element by name" + name);
}

QQuickItem* Hix::QML::findChildQItemByID(QQuickItem* start, const std::string& id)
{
	std::deque<QQuickItem*> s;
	s.push_back(start);
	QString str = QString::fromStdString(id);
	while (!s.empty())
	{
		auto curr = s.front();
		s.pop_front();
		auto context = qmlContext(curr);
		if (context && context->nameForObject(curr) == str)
		{
			return curr;
		}
		auto neighbors = curr->childItems();
		for (auto& each : neighbors)
		{
			s.push_back(each);
		}
	}
	throw std::runtime_error("failed to find QML element by id" + id);

}

QQuickItem* Hix::QML::findParendQItemByName(QQuickItem* start, const std::string& name)
{
	auto parent = start->parentItem();
	QString str = QString::fromStdString(name);
	while (parent != nullptr)
	{
		if (parent->objectName() == str)
		{
			return parent;
		}
		parent = parent->parentItem();
	}
	throw std::runtime_error("failed to find QML element by name" + name);
}

QQuickItem* Hix::QML::findParendQItemByID(QQuickItem* start, const std::string& id)
{
	auto parent = start->parentItem();
	QString str = QString::fromStdString(id);
	while (parent != nullptr)
	{
		auto context = qmlContext(parent);
		if (context && context->nameForObject(parent) == str)
		{
			return parent;
		}
		parent = parent->parentItem();
	}
	throw std::runtime_error("failed to find QML element by id" + id);

}
