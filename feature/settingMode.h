#pragma once
#include "interfaces/Mode.h"
#include "interfaces/DialogedMode.h"
#include <filesystem>

class GLModel;
namespace Hix
{
	namespace QML
	{
		namespace Controls
		{
			class DropdownBox;
			class Button;
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
			bool isDirty()const;
			Hix::QML::Controls::DropdownBox* _printerPresets;
			Hix::QML::Controls::InputSpinBox* _historySize;
			Hix::QML::Controls::Button* _profileBttn;
			Hix::QML::Controls::Button* _updateBttn;
			Hix::QML::Controls::Button* _resetBttn;

			std::deque<std::filesystem::path> _presetPaths; //same index as combo box
			int _oldIndex;
		};
	}
}

