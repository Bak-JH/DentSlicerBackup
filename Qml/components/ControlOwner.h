#pragma once
#include <unordered_map>
#include <string>
class QQuickItem;
namespace Hix
{
	namespace QML
	{
		//forward declare controls
		class InputControl;
		namespace Controls
		{
			class Button;
			class ToggleSwitch;
			class InputSpinBox;
			class TextInputBox;
			class DropdownBox;
		}
		class ControlOwner
		{
		public:
			virtual ~ControlOwner();
			void registerOwningControls();
			InputControl* getControlById(const std::string& id)const;
			//helper
			template<typename InputCtrlType>
			void getControl(InputCtrlType*& ctrlPtrOut, const std::string& id)const
			{
				static_assert(std::is_base_of<InputControl, InputCtrlType>{});
				ctrPtrOut = dynamic_cast<InputCtrlType*>(getControlById(id));
			}
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