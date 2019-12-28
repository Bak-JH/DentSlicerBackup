#pragma once
#include "../interfaces/Mode.h"`
#include "DentEngine/src/Bounds3D.h"
#include "../../render/PlaneMeshEntity.h"
class GLModel;
namespace Hix
{

	namespace Features
	{
		class ModelBuilderMode : public Hix::Features::Mode
		{
		public:
			ModelBuilderMode();
			virtual ~ModelBuilderMode();
		private:
			Hix::Render::PlaneMeshEntity _topPlane;
			Hix::Render::PlaneMeshEntity _bottPlane;
			std::unique_ptr<GLModel> _model;
		};
	}
}