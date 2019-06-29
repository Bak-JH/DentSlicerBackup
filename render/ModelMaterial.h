#pragma once
#include <Qt3dCore>
#include <Qt3DExtras/qt3dextras_global.h>
#include <Qt3DRender/qmaterial.h>
#include <Qt3DRender/qeffect.h>
#include <Qt3DRender/qtechnique.h>
#include <Qt3DRender/qshaderprogram.h>
#include <Qt3DRender/qshaderprogrambuilder.h>
#include <Qt3DRender/qparameter.h>
#include <Qt3DRender/qrenderpass.h>
#include <Qt3DRender/qgraphicsapifilter.h>
#include <QOpenGLShader>
namespace Hix
{
	namespace Render
	{
		// changeColor
		enum ModelColor
		{
			None = -1,
			Default = 0,
			Selected,
			OutOfBound,
			LayerMode
		};
		class ModelMaterial : public Qt3DRender::QMaterial
		{
			Q_OBJECT
		public:
			ModelMaterial();
			virtual ~ModelMaterial();

			//state machine
			void setMode();
			void setDiffuse(const QColor& diffuse);
			void setAmbient(const QColor& ambient);
			void addParameter(const std::string& key);
			void removeParameter(const std::string& key);
			void setParameterValue(const std::string& key, const QVariant& value);
			void changeColor(ModelColor mode);
		private:

			ModelColor colorMode = ModelColor::None;

			//custom parameters
			std::unordered_map<std::string, Qt3DRender::QParameter> _additionalParameters;

			//just to make sure the code compiles
			QOpenGLShader _vertShader;
			QOpenGLShader _fragShader;
			QOpenGLShader _geomShader;

			Qt3DRender::QEffect _effect;
			Qt3DRender::QTechnique _renderTechnique;
			Qt3DRender::QRenderPass _renderPass;
			Qt3DRender::QShaderProgram _shaderProgram;
			Qt3DRender::QFilterKey _filterKey;

			//default parameters
			Qt3DRender::QParameter _ambientParameter;
			Qt3DRender::QParameter _diffuseParameter;
	

		};
	}
}


