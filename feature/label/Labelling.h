#pragma once
#include "labelModel.h"
#include <QFont>
#include <QString>

namespace Hix
{
	namespace Features
	{
		class Labelling : public Feature
		{
		public:
			Labelling(GLModel* selectedModel);
			virtual ~Labelling();
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
			GLModel* _targetModel;
			Hix::Engine3D::Bounds3D _modelsBound;
		};
	}
}