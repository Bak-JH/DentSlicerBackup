#pragma once
#include "interfaces/Mode.h"
#include "../Tasking/Task.h"
#include "DentEngine/src/Bounds3D.h"
#include "interfaces/DialogedMode.h"
#include "../Qml/components/ControlForwardInclude.h"
#include "sliceExport.h"

class GLModel;
namespace Hix
{

	namespace Features
	{

		class SliceExportGPU : public Hix::Tasking::Task
		{
		public:
			SliceExportGPU(const std::unordered_set<GLModel*>& selected, QString path);
			void run()override;

		private:
			std::unordered_set<GLModel*> _models;
			QString _path;

		};
	}
}