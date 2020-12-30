#pragma once
#include <QFont>
#include <QString>
#include <memory>
#include "../interfaces/SelectFaceMode.h"
#include "../interfaces/FlushSupport.h"
#include "../interfaces/DialogedMode.h"
#include "../../DentEngine/src/Bounds3D.h"
#include "../../Qml/components/ControlForwardInclude.h"

class CorkTriMesh;
class GLModel;
namespace Hix
{
	namespace Engine3D
	{
		class RayCaster;
		class RayAccelerator;
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
			void applyButtonClicked()override;

		private:
			Hix::QML::Controls::TextInputBox* _inputText;
			Hix::QML::Controls::DropdownBox* _fontStyle;
			Hix::QML::Controls::InputSpinBox* _fontSize;
			Hix::QML::Controls::InputSpinBox* _labelDepth;
			Hix::QML::Controls::ToggleSwitch* _isEmboss; //engrave
			//QFont _font = QFont("Arial", 12, QFont::Normal);
			std::unique_ptr<GLModel> _previewModel;
			GLModel* _targetModel = nullptr;
			Hix::Engine3D::Bounds3D _modelsBound;
			GLModel* generatePreviewModel(const QMatrix4x4& transformMat);
			//bool _isDirty = true;
			
			QMatrix4x4 _matrix;
			QVector3D _scale;
			std::unordered_map<const GLModel*, std::unique_ptr<Hix::Engine3D::RayAccelerator>> _rayAccels;
			std::unique_ptr<Hix::Engine3D::RayCaster> _rayCaster;

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
			std::variant<GLModel*, std::unique_ptr<GLModel>> _label;
			GLModel* _targetModel;
		};

		class LabellingEngrave : public Feature, public FlushSupport
		{
		public:
			LabellingEngrave(GLModel* parentModel, GLModel* previewModel);
		protected:
			void runImpl()override;
			void undoImpl()override;
			void redoImpl()override;
		private:
			void cutCSG(GLModel* subject, const CorkTriMesh& subtract);
			GLModel* _target;
			std::unique_ptr<GLModel> _label;
			std::unique_ptr<Hix::Engine3D::Mesh> _prevMesh;

		};



	}
}