#pragma once
#include "feature/interfaces/DialogedMode.h"
#include "feature/interfaces/FlushSupport.h"
#include <qvector3d.h>
#include <qmatrix4x4.h>
#include "../DentEngine/src/Bounds3D.h"
namespace Hix
{
	namespace QML
	{
		namespace Controls
		{
			class InputSpinBox;
		}
	}

	namespace Features
	{
		class Scale : public Feature
		{
		public:
			Scale(GLModel* targetModel, QVector3D& scale);
			virtual ~Scale();
		protected:
			void undoImpl()override;
			void redoImpl()override;
			void runImpl()override;

		private:
			QVector3D _scale;
			GLModel* _model;
			QMatrix4x4 _prevMatrix;
			Engine3D::Bounds3D _prevAabb;
			QMatrix4x4 _nextMatrix;
			Engine3D::Bounds3D _nextAabb;
		};

		class ScaleMode : public DialogedMode
		{
		public:
			ScaleMode();
			virtual ~ScaleMode();

			FeatureContainerFlushSupport* applyScale(QVector3D scale);
			void apply()override;
		private:
			std::unordered_set<GLModel*> _targetModels;
			Hix::QML::Controls::InputSpinBox* _precentValue;
			Hix::QML::Controls::InputSpinBox* _xValue;
			Hix::QML::Controls::InputSpinBox* _yValue;
			Hix::QML::Controls::InputSpinBox* _zValue;
		};

	}
}