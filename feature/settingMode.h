#pragma once
#include "interfaces/Mode.h"
#include "interfaces/DialogedMode.h"
class GLModel;
namespace Hix
{
	namespace QML
	{
		namespace Controls
		{
			class InputSpinBox;
		}
	}

	namespace Features
	{
		class SettingMode: public DialogedMode
		{
		public:
			SettingMode();
			virtual ~SettingMode();
			void applyButtonClicked()override;


		private:
			Hix::QML::Controls::InputSpinBox* _extendValue;
		};
	}
}

