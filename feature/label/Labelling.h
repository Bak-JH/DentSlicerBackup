#pragma once
#include "labelModel.h"
#include <QFont>
#include <QString>
#include "../interfaces/SelectFaceFeature.h"
namespace Hix
{
	namespace Features
	{
		class Labelling : public SelectFaceFeature
		{
		public:
			Labelling();
			virtual ~Labelling();
			void faceSelected(GLModel* selected, const Hix::Engine3D::FaceConstItr& selectedFace, const Hix::Input::MouseEventData& mouse, const Qt3DRender::QRayCasterHit& hit)override;

			void updateLabelMesh(const QVector3D translation, const QVector3D normal);
			void setText(const QString text);
			void setFontName(const QString fontName);
			void setFontBold(const bool isBold);
			void setFontSize(const int fontSize);
			void generateLabelMesh();

		private:
			QString _text = "Enter text";
			QFont _font = QFont("Arial", 12, QFont::Normal);
			Hix::LabelModel* _previewModel = nullptr;
			GLModel* _targetModel = nullptr;
			Hix::Engine3D::Bounds3D _modelsBound;
		};
	}
}