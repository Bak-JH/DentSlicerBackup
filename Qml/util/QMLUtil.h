#pragma once
#include <string>
class QQuickItem;
class QString;
namespace Hix
{
	namespace QML
	{
		QQuickItem* findChildQItemByName(QQuickItem* start, const std::string& name);
		QQuickItem* findChildQItemByID(QQuickItem* start, const std::string& id);
		QQuickItem* findParendQItemByName(QQuickItem* start, const std::string& name);
		QQuickItem* findParendQItemByID(QQuickItem* start, const std::string& id);


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

		template<typename ItemType>
		void getParentByName(QQuickItem* start, ItemType*& ctrlPtrOut, const std::string& name)
		{
			static_assert(std::is_base_of<QQuickItem, ItemType>{});
			ctrlPtrOut = dynamic_cast<ItemType*>(findParendQItemByName(start, name));
		}
		template<typename ItemType>
		void getParentByID(QQuickItem* start, ItemType*& ctrlPtrOut, const std::string& id)
		{
			static_assert(std::is_base_of<QQuickItem, ItemType>{});
			ctrlPtrOut = dynamic_cast<ItemType*>(findParendQItemByID(start, id));
		}
	}
}