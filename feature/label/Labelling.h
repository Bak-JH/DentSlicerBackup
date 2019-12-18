#pragma once
#include "../../glmodel.h"
#include <QFont>
#include <QString>
#include "../interfaces/SelectFaceMode.h"
namespace Hix
{
	namespace Features
	{
		class LabellingMode : public SelectFaceMode
		{
		public:
			LabellingMode();
			virtual ~LabellingMode();
			void faceSelected(GLModel* selected, const Hix::Engine3D::FaceConstItr& selectedFace, const Hix::Input::MouseEventData& mouse, const Qt3DRender::QRayCasterHit& hit)override;
			void updateLabelMesh(const QVector3D& translation, const Hix::Engine3D::FaceConstItr& face);
			void setText(const QString& text);
			void setFontName(const QString& fontName);
			void setFontBold(bool isBold);
			void setFontSize(int fontSize);
			Hix::Features::Feature* applyLabelMesh();

		private:
			QString _text = "Enter text";
			QFont _font = QFont("Arial", 12, QFont::Normal);
			std::unique_ptr<GLModel> _previewModel;
			GLModel* _targetModel = nullptr;
			Hix::Engine3D::Bounds3D _modelsBound;
			GLModel* generatePreviewModel();
			bool _isDirty = true;
			
			QMatrix4x4 _matrix;
			QVector3D _scale;
		};

		class Labelling : public Feature
		{
		public:
			Labelling(GLModel* parentModel, GLModel* previewModel);
			virtual ~Labelling();
			void undo()override;
			void redo()override;

		private:
			GLModel* _label;
			GLModel* _targetModel;
		};
	}
}