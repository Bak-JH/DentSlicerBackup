#pragma once
#include "feature/interfaces/FlushSupport.h"
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
			Rotate(GLModel* target, const QQuaternion& rot);
			virtual ~Rotate();
			void undoImpl()override;
			void redoImpl()override;

		private:
			GLModel* _model;
			QMatrix4x4 _prevMatrix;
			Engine3D::Bounds3D _prevAabb;
			QMatrix4x4 _nextMatrix;
			Engine3D::Bounds3D _nextAabb;
		};

		class RotateMode : public WidgetMode
		{
		public:
			RotateMode(const std::unordered_set<GLModel*>& targetModels, Input::RayCastController* controller);
			virtual ~RotateMode();
			void featureStarted()override;
			void featureEnded()override;
			Hix::Features::FeatureContainerFlushSupport* applyRotate(const QQuaternion& rot);

		private:
			Hix::Features::FeatureContainerFlushSupport* _rotateContainer;
		};

		
	}
}