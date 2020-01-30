#pragma once
class QQuickItem;
class QString;
namespace Hix
{
	namespace QML
	{
		QQuickItem* findChildItemByName(QQuickItem* start, const QString& name);
		QQuickItem* findChildItemByID(QQuickItem* start, const QString& id);

	}
}