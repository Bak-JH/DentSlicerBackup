#pragma once
#include "../interfaces/Mode.h"
#include "../Tasking/Task.h"
#include "render/Bounds3D.h"
#include "../interfaces/DialogedMode.h"
#include "../Qml/components/ControlForwardInclude.h"
#include "sliceExport.h"

class GLModel;
namespace Hix
{
	namespace Engine3D
	{
		class Bounds3D;
	}

	namespace Features
	{

		class SliceExportGPU : public Hix::Tasking::Task
		{
		public:
			SliceExportGPU(const std::unordered_set<GLModel*>& selected, QString path);
			void run()override;
			std::vector<float> toVtxBuffer(Hix::Engine3D::Bounds3D& bounds);

		private:
			struct SlicerArgs
			{
				float delta;
				size_t sampleXY;
				size_t sampleZ;
				float minHeight;
				float maxHeight;
				float pixelWidth;
				size_t imgX;
				size_t imgY;
				size_t vtcCount;
			};

			std::unordered_set<GLModel*> _models;
			QString _path;

		};
	}
}