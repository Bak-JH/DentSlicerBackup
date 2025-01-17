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
#include <QBuffer>

#include "Color.h"


class GLModel;
namespace Hix
{
	namespace Render
	{
		// changeColor
		enum ShaderMode
		{
			None = -1,
			SingleColor = 0,
			PerPrimitiveColor,
			LayerMode,
			NoLightsMode
		};
		class ModelMaterial : public Qt3DRender::QMaterial
		{
			Q_OBJECT
		public:
			ModelMaterial(Qt3DCore::QNode* parent = nullptr);
			virtual ~ModelMaterial();

			void addParameterWithKey(const std::string& key);
			void removeParameterWithKey(const std::string& key);
			void setParameterValue(const std::string& key, const QVariant& value);
			void changeMode(ShaderMode mode);
			ShaderMode shaderMode()const;
			//color for single color mode
			void setColor(QVector4D color);
			void setColor(QColor color);
			void setDiffuse(const QColor& diffuse);
			void setAmbient(const QColor& ambient);
		private:
			ShaderMode _mode = ShaderMode::None;
			//custom parameters
			std::unordered_map<std::string, Qt3DRender::QParameter> _additionalParameters;

			Qt3DRender::QEffect _effect;

			//default parameters
			Qt3DRender::QParameter _singleColorParameter;
			Qt3DRender::QParameter _ambientParameter;
			Qt3DRender::QParameter _diffuseParameter;
			Qt3DRender::QFilterKey _filterKey;
			Qt3DRender::QRenderPass _renderPass;
			Qt3DRender::QShaderProgram _shaderProgram;
			Qt3DRender::QTechnique _renderTechnique;

	


			//per primtivie parameters, stored as QVariantList on CPU, uniform on GPU geomatry shader

		};
	}
}


