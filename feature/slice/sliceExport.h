#pragma once
#include "../interfaces/Mode.h"
#include "../Tasking/Task.h"
#include "render/Bounds3D.h"
#include "../interfaces/DialogedMode.h"
#include "../Qml/components/ControlForwardInclude.h"

class GLModel;
namespace Hix
{

	namespace Features
	{
		class SliceExportMode : public DialogedMode
		{
		public:
			SliceExportMode();
			virtual ~SliceExportMode();
			void applyButtonClicked()override;
			void applyAndClose()override;
		private:
			//UI slicing options
			Hix::QML::Controls::DropdownBox* _sliceTypeDrop;
			Hix::QML::Controls::InputSpinBox* _layerHeightSpin;
			Hix::QML::Controls::InputSpinBox* _aaxySpin;
			Hix::QML::Controls::InputSpinBox* _aazSpin;

			Hix::QML::Controls::ToggleSwitch* _invertXSwtch;


			static void applySettings();

		};

		//class SliceExport : public Hix::Tasking::Task
		//{
		//public:
		//	SliceExport(const std::unordered_set<GLModel*>& selected, QString path);
		//	void run()override;

		//private:
		//	std::unordered_set<GLModel*> _models;
		//	QString _path;

		//};
	}
}