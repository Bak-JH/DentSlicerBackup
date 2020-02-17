#pragma once
#include <string>
class QObject;
class QString;
namespace Hix
{
	namespace QML
	{
		QObject* findChildQItemByName(QObject* start, const std::string& name);
		QObject* findChildQItemByID(QObject* start, const std::string& id);
		QObject* findParendQItemByName(QObject* start, const std::string& name);
		QObject* findParendQItemByID(QObject* start, const std::string& id);


		template<typename ItemType>
		void getItemByName(QObject* start, ItemType*& ctrlPtrOut, const std::string& name)
		{
			static_assert(std::is_base_of<QObject, ItemType>{});
			ctrlPtrOut = dynamic_cast<ItemType*>(findChildQItemByName(start, name));
		}
		template<typename ItemType>
		void getItemByID(QObject* start, ItemType*& ctrlPtrOut, const std::string& id)
		{
			static_assert(std::is_base_of<QObject, ItemType>{});
			ctrlPtrOut = dynamic_cast<ItemType*>(findChildQItemByID(start, id));
		}

		template<typename ItemType>
		void getParentByName(QObject* start, ItemType*& ctrlPtrOut, const std::string& name)
		{
			static_assert(std::is_base_of<QObject, ItemType>{});
			ctrlPtrOut = dynamic_cast<ItemType*>(findParendQItemByName(start, name));
		}
		template<typename ItemType>
		void getParentByID(QObject* start, ItemType*& ctrlPtrOut, const std::string& id)
		{
			static_assert(std::is_base_of<QObject, ItemType>{});
			ctrlPtrOut = dynamic_cast<ItemType*>(findParendQItemByID(start, id));
		}
	}
}