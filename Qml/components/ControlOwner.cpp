#include "ControlOwner.h"
#include "InputControl.h"
#include <qquickitem.h>
#include <deque>
#include <qqmlcontext.h>
using namespace  Hix::QML;




Hix::QML::ControlOwner::~ControlOwner()
{
}


std::unordered_map<std::string, InputControl*> BFSAllItems(QQuickItem* start)
{
	std::unordered_map<std::string, InputControl*> map;
	std::deque<QQuickItem*> s;
	s.emplace_back(start);

	while (!s.empty())
	{
		auto curr = s.front();
		s.pop_front();

		auto neighbors = curr->childItems();
		for (auto& each : neighbors)
		{
			auto inputControl = dynamic_cast<InputControl*>(each);
			if (inputControl)
			{
				auto context = qmlContext(each);
				if (context)
				{
					auto qID = context->nameForObject(each);
					map.emplace(qID.toStdString(), inputControl);
				}
			}
			s.emplace_back(each);
		}
	}
	return map;

}

void Hix::QML::ControlOwner::registerOwningControls()
{
	_inputControls = BFSAllItems(getQItem());
}

InputControl* Hix::QML::ControlOwner::getControlById(const std::string& id) const
{
	auto result = _inputControls.find(id);
	if (result != _inputControls.cend())
	{
		return result->second;
	}
	return nullptr;
}

void Hix::QML::ControlOwnerAttorny::addControl(ControlOwner* owner, InputControl& control)
{
}

void Hix::QML::ControlOwnerAttorny::removeControl(ControlOwner* owner, InputControl& control)
{
}
