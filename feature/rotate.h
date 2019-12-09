#pragma once
#include "feature/interfaces/Feature.h"
#include "feature/interfaces/WidgetMode.h"
#include "ui/Widget3D.h"
#include "DentEngine/src/Bounds3D.h"

namespace Hix
{
	namespace Features
	{
		class Rotate : public Feature
		{
		public:
			Rotate(GLModel* target);
			virtual ~Rotate();
			void undo()override;
			void redo()override;

		private:
			GLModel* _target;
			QMatrix4x4 _prevMatrix;
			Engine3D::Bounds3D _prevAabb;
		};

		class RotateMode : public WidgetMode
		{
		public:
			RotateMode(const std::unordered_set<GLModel*>& targetModels, Input::RayCastController* controller);
			virtual ~RotateMode();
			void featureStarted()override;
			void featureEnded()override;

		private:
			Hix::Features::FeatureContainer* _rotateContainer;
		};

		
	}
}