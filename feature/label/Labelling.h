#pragma once
#include "labelModel.h"

namespace Hix
{
	namespace Features
	{
		class Labelling : public Feature
		{
		public:
			Labelling(const std::unordered_set<GLModel*>& selectedModels);
			void setTranslation(QVector3D translation);
			void setText(QString text);
			void setFontName(QString fontName);
			void setFontBold(bool isBold);
			void setFontSize(int fontSize);
			void generateLabelMesh();

		private:
			Hix::LabelModel* _previewModel;
			std::unordered_set<GLModel*> _models;
			Hix::Engine3D::Bounds3D _modelsBound;
		};
	}
}