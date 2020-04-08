#pragma once
#include "../interfaces/DialogedMode.h"
#include "../interfaces/WidgetMode.h"
#include "../interfaces/RangeSliderMode.h"

#include "DentEngine/src/Bounds3D.h"
#include "../../render/PlaneMeshEntity.h"
#include "../rotate.h"
class GLModel;
namespace Hix
{

	namespace Features
	{
		class TwoManifoldBuilder;
		class ModelBuilderMode : public Hix::Features::DialogedMode, public Hix::Features::WidgetMode, public Hix::Features::RangeSliderMode
		{
		public:
			ModelBuilderMode();
			virtual ~ModelBuilderMode();
			void build();
			void applyButtonClicked()override;
			void featureStarted()override;
			void featureEnded()override;
			QVector3D getWidgetPosition()override;

		private:
			Hix::Features::FeatureContainerFlushSupport* _rotateContainer;
			Hix::Render::PlaneMeshEntity _topPlane;
			Hix::Render::PlaneMeshEntity _bottPlane;
			std::unique_ptr<GLModel> _model;
			friend class MBPrep;
		};

		class MBPrep : public Hix::Tasking::Task
		{
		public:
			MBPrep(ModelBuilderMode* mode, QUrl fileUrl);
			virtual ~MBPrep();
			void run()override;
		private:
			ModelBuilderMode* _mode;
			QUrl _fileUrl;
		};

	}
}


