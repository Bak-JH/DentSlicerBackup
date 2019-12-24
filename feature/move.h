#pragma once
#include "feature/interfaces/FlushSupport.h"
#include "feature/interfaces/WidgetMode.h"
#include "ui/Widget3D.h"
#include "DentEngine/src/Bounds3D.h"

namespace Hix
{
	namespace Features
	{
		class Move : public Feature
		{
		public:
			Move(GLModel* target, const QVector3D& to);
			Move(GLModel* target);
			virtual ~Move();
			void undoImpl()override;
			void redoImpl()override;

		private:
			GLModel* _model;
			QMatrix4x4 _prevMatrix;
			Engine3D::Bounds3D _prevAabb;
			QMatrix4x4 _nextMatrix;
			Engine3D::Bounds3D _nextAabb;
		};

		class MoveMode : public WidgetMode
		{
		public:
			MoveMode(const std::unordered_set<GLModel*>& targetModels, Input::RayCastController* controller);
			virtual ~MoveMode();
			void featureStarted()override;
			void featureEnded()override;
			FeatureContainerFlushSupport* applyMove(const QVector3D& to);
		private:
			Hix::Features::FeatureContainerFlushSupport* _moveContainer = nullptr;
		};
	}
}