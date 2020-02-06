#include "QMLUtil.h"
#include <deque>
#include <qquickitem.h>
#include <qstring.h>
#include <qqmlcontext.h>
#include "../../qmlmanager.h"


QQuickItem* Hix::QML::findChildQItemByName(QQuickItem* start, const QString& name)
{
	std::deque<QQuickItem*> s;
	s.push_back(start);

	while (!s.empty())
	{
		auto curr = s.front();
		s.pop_front();
		if (curr->objectName() == name)
		{
			return curr;
		}
		auto neighbors = curr->childItems();
		for (auto& each : neighbors)
		{
			s.push_back(each);
		}
	}
	throw std::runtime_error("failed to find QML element by name" + name.toStdString());
}

QQuickItem* Hix::QML::findChildQItemByID(QQuickItem* start, const QString& id)
{
	std::deque<QQuickItem*> s;
	s.push_back(start);
	while (!s.empty())
	{
		auto curr = s.front();
		s.pop_front();
		auto context = qmlContext(curr);
		if (context && context->nameForObject(curr) == id)
		{
			return curr;
		}
		auto neighbors = curr->childItems();
		for (auto& each : neighbors)
		{
			s.push_back(each);
		}
	}
	throw std::runtime_error("failed to find QML element by id" + id.toStdString());

}

QQuickItem* Hix::QML::findParendQItemByName(QQuickItem* start, const QString& name)
{
	auto parent = start->parentItem();
	while (parent != nullptr)
	{
		if (parent->objectName() == name)
		{
			return parent;
		}
		parent = parent->parentItem();
	}
	throw std::runtime_error("failed to find QML element by name" + name.toStdString());
}

QQuickItem* Hix::QML::findParendQItemByID(QQuickItem* start, const QString& id)
{
	auto parent = start->parentItem();
	while (parent != nullptr)
	{
		auto context = qmlContext(parent);
		if (context && context->nameForObject(parent) == id)
		{
			return parent;
		}
		parent = parent->parentItem();
	}
	throw std::runtime_error("failed to find QML element by id" + id.toStdString());

}
