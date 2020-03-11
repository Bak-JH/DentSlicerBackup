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
			GLModel* _model;
		private:
			QMatrix4x4 _prevMatrix;
			Engine3D::Bounds3D _prevAabb;
			QMatrix4x4 _nextMatrix;
			Engine3D::Bounds3D _nextAabb;
		};
		struct Move::ZToBed : public Move
		{
			ZToBed(GLModel* target);
		};

		class MoveMode : public WidgetMode, public DialogedMode
		{
		public:

			MoveMode();
			virtual ~MoveMode();
			void featureStarted()override;
			void featureEnded()override;
			QVector3D getWidgetPosition()override;
			void applyButtonClicked()override;
			void modelMoveWithAxis(QVector3D axis, double distance);
			void modelMove(QVector3D displacement);
			FeatureContainerFlushSupport* applyMove(const QVector3D& to);

		private:
			Hix::Features::FeatureContainerFlushSupport* _moveContainer = nullptr;
			std::unordered_set<GLModel*> _targetModels;
			Hix::QML::Controls::InputSpinBox* _xValue;
			Hix::QML::Controls::InputSpinBox* _yValue;
			Hix::QML::Controls::InputSpinBox* _zValue;
			Hix::QML::Controls::Button* _snapButton;
		};
	}
}