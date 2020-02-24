#pragma once
#include "feature/interfaces/FlushSupport.h"
#include "feature/interfaces/WidgetMode.h"
#include "widget/Widget3D.h"
#include "DentEngine/src/Bounds3D.h"
#include "interfaces/DialogedMode.h"
#include <optional>
class GLModel;
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
		class Rotate : public Feature
		{
		public:
			Rotate(GLModel* target);
			Rotate(GLModel* target, const QQuaternion& rot);
			virtual ~Rotate();
			const GLModel* model()const;
		protected:
			void undoImpl()override;
			void redoImpl()override;
			void runImpl()override;

		private:
			std::optional<QQuaternion> _rot;
			GLModel* _model;
			QMatrix4x4 _prevMatrix;
			Engine3D::Bounds3D _prevAabb;
			QMatrix4x4 _nextMatrix;
			Engine3D::Bounds3D _nextAabb;
		};

		class RotateMode : public WidgetMode, public DialogedMode
		{
		public:
			RotateMode();
			virtual ~RotateMode();
			void featureStarted()override;
			void featureEnded()override;
			QVector3D getWidgetPosition()override;
			std::unordered_set<GLModel*>& models();
			void apply()override;

		protected:
			Hix::Features::FeatureContainerFlushSupport* _rotateContainer;
			std::unordered_set<GLModel*> _targetModels;

		private:
			Hix::QML::Controls::InputSpinBox* _xValue;
			Hix::QML::Controls::InputSpinBox* _yValue;
			Hix::QML::Controls::InputSpinBox* _zValue;

		};

		class RotateModeNoUndo : public RotateMode
		{
		public:
			RotateModeNoUndo(const std::unordered_set<GLModel*>& targetModels);
			virtual ~RotateModeNoUndo();
			void featureStarted()override;
			void featureEnded()override;
		};

		
	}
}