#pragma once
#include <unordered_map>
#include <string>
class QQuickItem;
namespace Hix
{
	namespace QML
	{

		class InputControl;
		class ControlOwner
		{
		public:
			virtual ~ControlOwner();
			void registerOwningControls();
			InputControl* getControlById(const std::string& id)const;
		protected:
			virtual QQuickItem* getQItem() = 0;
		private:
			std::unordered_map<std::string, InputControl*> _inputControls;
			friend class ControlOwnerAttorny;
		};
		class ControlOwnerAttorny
		{
		private:
			//static std::unordered_map<std::string, InputControl*>& controlMap(ControlOwner* owner);
			static void addControl(ControlOwner* owner, InputControl& control);
			static void removeControl(ControlOwner* owner, InputControl& control);

		};
	}
}