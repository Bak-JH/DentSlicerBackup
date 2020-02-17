#pragma once
#include <QFont>
#include <QString>
#include "../interfaces/SelectFaceMode.h"
#include "../interfaces/FlushSupport.h"
#include "../interfaces/DialogedMode.h"
#include "../../DentEngine/src/Bounds3D.h"
namespace Hix
{
	namespace QML
	{
		namespace Controls
		{
			class TextInputBox;
			class InputSpinBox;
			class DropdownBox;
			class ToggleSwitch;
		}
	}

	namespace Features
	{
		class LabellingMode : public SelectFaceMode, public DialogedMode
		{
		public:
			LabellingMode();
			virtual ~LabellingMode();
			void faceSelected(GLModel* selected, const Hix::Engine3D::FaceConstItr& selectedFace, const Hix::Input::MouseEventData& mouse, const Qt3DRender::QRayCasterHit& hit)override;
			void updateLabelMesh(const QVector3D& translation, const Hix::Engine3D::FaceConstItr& face);
			Hix::Features::Feature* applyLabelMesh();

		private:
			Hix::QML::Controls::TextInputBox* _inputText;
			Hix::QML::Controls::DropdownBox* _fontStyle;
			Hix::QML::Controls::InputSpinBox* _fontSize;
			Hix::QML::Controls::InputSpinBox* _labelHeight;
			Hix::QML::Controls::InputSpinBox* _labelType;
			//QFont _font = QFont("Arial", 12, QFont::Normal);
			std::unique_ptr<GLModel> _previewModel;
			GLModel* _targetModel = nullptr;
			Hix::Engine3D::Bounds3D _modelsBound;
			GLModel* generatePreviewModel();
			bool _isDirty = true;
			
			QMatrix4x4 _matrix;
			QVector3D _scale;
		};

		class Labelling : public Feature, public FlushSupport
		{
		public:
			Labelling(GLModel* parentModel, GLModel* previewModel);
			virtual ~Labelling();
		protected:
			void undoImpl()override;
			void redoImpl()override;
			void runImpl()override;
		private:
			GLModel* _label;
			GLModel* _targetModel;
		};
	}
}