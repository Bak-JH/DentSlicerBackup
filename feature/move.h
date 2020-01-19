#pragma once
#include "feature/interfaces/FlushSupport.h"
#include "feature/interfaces/WidgetMode.h"
#include "widget/Widget3D.h"
#include "DentEngine/src/Bounds3D.h"
#include <optional>

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
			struct ZToBed;
		protected:
			void undoImpl()override;
			void redoImpl()override;
			void runImpl()override;
			std::optional<QVector3D> _to;
		private:
			GLModel* _model;
			QMatrix4x4 _prevMatrix;
			Engine3D::Bounds3D _prevAabb;
			QMatrix4x4 _nextMatrix;
			Engine3D::Bounds3D _nextAabb;
		};
		struct Move::ZToBed : public Move
		{
			ZToBed(GLModel* target);
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