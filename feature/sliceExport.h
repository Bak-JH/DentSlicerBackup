#pragma once
#include "interfaces/Mode.h"`
#include "../Tasking/Task.h"
#include "DentEngine/src/Bounds3D.h"
#include "../slice/slicingengine.h"
class GLModel;
namespace Hix
{

	namespace Features
	{
		class SliceExportMode : public Hix::Features::Mode
		{
		public:
			SliceExportMode();
			virtual ~SliceExportMode();
		private:
		};

		class SliceExport : public Hix::Tasking::Task
		{
		public:
			SliceExport(const std::unordered_set<GLModel*>& selected, QString path);
			void run()override;
		private:
			std::unordered_set<GLModel*> _models;
			QString _path;

		};
	}
}