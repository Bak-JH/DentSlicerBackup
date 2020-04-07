#pragma once
#include "../interfaces/DialogedMode.h"`
#include "DentEngine/src/Bounds3D.h"
#include "../../render/PlaneMeshEntity.h"
#include "../rotate.h"
class GLModel;
namespace Hix
{

	namespace Features
	{
		class TwoManifoldBuilder;
		class ModelBuilderMode : public Hix::Features::DialogedMode
		{
		public:
			ModelBuilderMode();
			virtual ~ModelBuilderMode();
			void build();
			void getSliderSignalTop(double value);
			void getSliderSignalBot(double value);
			void applyButtonClicked()override;

		private:
			std::unique_ptr<RotateModeNoUndo> _rotateMode;
			Hix::Render::PlaneMeshEntity _topPlane;
			Hix::Render::PlaneMeshEntity _bottPlane;
			std::unique_ptr<GLModel> _model;
			float _zLength;
		};
	}
}