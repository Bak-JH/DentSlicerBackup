#include "QMLUtil.h"
#include <deque>
#include <qquickitem.h>
#include <qstring.h>
#include <qqmlcontext.h>



QQuickItem* Hix::QML::findChildItemByName(QQuickItem* start, const QString& name)
{
	std::deque<QQuickItem*> s;
	s.push_back(start);

	while (!s.empty())
	{
		auto curr = s.back();
		s.pop_back();
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
	return nullptr;
}

QQuickItem* Hix::QML::findChildItemByID(QQuickItem* start, const QString& id)
{
	std::deque<QQuickItem*> s;
	s.push_back(start);

	while (!s.empty())
	{
		auto curr = s.back();
		s.pop_back();
		QQmlContext* const context = qmlContext(curr);
		if (context->nameForObject(curr) == id)
		{
			return curr;
		}
		auto neighbors = curr->childItems();
		for (auto& each : neighbors)
		{
			s.push_back(each);
		}
	}
	return nullptr;

}
