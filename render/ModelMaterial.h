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
		class ModelMaterial : public Qt3DRender::QMaterial
		{
			Q_OBJECT
		public:
			ModelMaterial();
			virtual ~ModelMaterial();
		private:

			//parameters
			Qt3DRender::QParameter _ambientParameter;
			Qt3DRender::QParameter _diffuseParameter;
			Qt3DRender::QParameter _specularParameter;
			Qt3DRender::QParameter _pointLightPositionsParameter; //multiple positions
			//this is hard coded in to shader to make the for loop in shader a const bound loop.
			//Qt3DRender::QParameter _pointLightCountParameter; //multiple positions

			//since it's white color, no calculation is needed, so skipped for performance
			//Qt3DRender::QParameter _pointLightColorParameter; //single color



			//just to make sure the code compiles
			QOpenGLShader _vertShader;
			QOpenGLShader _fragShader;
			QOpenGLShader _geomShader;

			Qt3DRender::QEffect _effect;
			Qt3DRender::QTechnique _renderTechnique;
			Qt3DRender::QRenderPass _renderPass;
			Qt3DRender::QShaderProgram _shaderProgram;
			//Qt3DRender::QShaderProgramBuilder _programBuilder;
			Qt3DRender::QFilterKey _filterKey;
			//lighting calculation for model:
			//Assume 4 spot lights in z =0; and x,y= _lightDistance away.
			//for lighting information
			float _lightDistance = 100;
			//size_t _numberOfLights = 4;
			//QVector3D _lightColor = { 1.0, 1.0, 1.0 };


		};
	}
}


