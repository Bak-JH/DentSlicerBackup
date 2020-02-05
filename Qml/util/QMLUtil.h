#pragma once
#include <string>
class QQuickItem;
class QString;
namespace Hix
{
	namespace QML
	{
		QQuickItem* findChildQItemByName(QQuickItem* start, const QString& name);
		QQuickItem* findChildQItemByID(QQuickItem* start, const QString& id);

		template<typename ItemType>
		void getItemByName(QQuickItem* start, ItemType*& ctrlPtrOut, const std::string& name)
		{
			static_assert(std::is_base_of<QQuickItem, ItemType>{});
			ctrlPtrOut = dynamic_cast<ItemType*>(findChildQItemByName(start, name));
		}

		template<typename ItemType>
		void getItemByID(QQuickItem* start, ItemType*& ctrlPtrOut, const std::string& id)
		{
			static_assert(std::is_base_of<QQuickItem, ItemType>{});
			ctrlPtrOut = dynamic_cast<ItemType*>(findChildQItemByID(start, id));
		}
	}
}