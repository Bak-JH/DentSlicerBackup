#pragma once
#include "qmlmanager.h"
#include "feature/interfaces/Mode.h"
#include "feature/interfaces/FlushSupport.h"

namespace Hix
{
	namespace Features
	{
		class Scale : public Feature
		{
		public:
			Scale(GLModel* targetModel, QVector3D& scale);
			virtual ~Scale();
			void undo()override;
			void redo()override;

		private:
			std::unique_ptr<GLModel> _model;
			QMatrix4x4 _prevMatrix;
			Engine3D::Bounds3D _prevAabb;
			QMatrix4x4 _nextMatrix;
			Engine3D::Bounds3D _nextAabb;
		};

		class ScaleMode : public Mode
		{
		public:
			ScaleMode(const std::unordered_set<GLModel*>& selectedModels);
			virtual ~ScaleMode();

			FeatureContainerFlushSupport* applyScale(QVector3D scale);
		private:
			std::unordered_set<GLModel*> _targetModels;
		};

	}
}