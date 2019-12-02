#pragma once
#include "qmlmanager.h"
#include "feature/interfaces/Mode.h"
#include "feature/interfaces/Feature.h"

namespace Hix
{
	namespace Features
	{
		class ScaleMode : public Mode
		{
		public:
			ScaleMode(const std::unordered_set<GLModel*>& selectedModels);
			virtual ~ScaleMode();

			std::unique_ptr<FeatureContainer> applyScale(QVector3D scale);
		private:
			std::unordered_set<GLModel*> _targetModels;
		};

		class Scale : public Feature
		{
		public:
			Scale(GLModel* targetModel, QVector3D& scale);
			virtual ~Scale();
			void undo()override;

		private:
			GLModel* _model;
			QVector3D _prevScale;
		};
	}
}