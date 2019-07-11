#include "ModelMaterial.h"
#include <Qt3DRender>
#include <stdexcept>
using namespace Qt3DRender;
using namespace Hix::Render;
const QString VERT_PATH = "qrc:/shaders/model_shader.vert";
const QString GEOM_PATH = "qrc:/shaders/model_shader.geom";
const QString FRAG_PATH = "qrc:/shaders/model_shader.frag";

Hix::Render::ModelMaterial::ModelMaterial():
	_ambientParameter(QStringLiteral("ambient"), QColor(25, 25, 25)),
	_diffuseParameter(QStringLiteral("diffuse"), QColor(80, 80, 80)),
	_vertShader(QOpenGLShader::Vertex),
	_fragShader(QOpenGLShader::Fragment),
	_geomShader(QOpenGLShader::Geometry)
{

	//GLSL in this context can use following uniforms
	//Above declared parameters
	//QT defined uniforms, see QShaderProgram documentation


	_shaderProgram.setVertexShaderCode(QShaderProgram::loadSource(QUrl(VERT_PATH)));
	_shaderProgram.setGeometryShaderCode(QShaderProgram::loadSource(QUrl(GEOM_PATH)));
	_shaderProgram.setFragmentShaderCode(QShaderProgram::loadSource(QUrl(FRAG_PATH)));
	_renderPass.setShaderProgram(&_shaderProgram);

	auto cullFace = new QCullFace(&_renderPass);
	cullFace->setMode(QCullFace::CullingMode::Back);
	_renderPass.addRenderState(cullFace);

	auto depthTest = new QDepthTest(&_renderPass);
	depthTest->setDepthFunction(QDepthTest::DepthFunction::Less);
	_renderPass.addRenderState(depthTest);
	_renderTechnique.addRenderPass(&_renderPass);
	_renderTechnique.graphicsApiFilter()->setApi(QGraphicsApiFilter::OpenGL);
	_renderTechnique.graphicsApiFilter()->setMajorVersion(3);
	_renderTechnique.graphicsApiFilter()->setMinorVersion(3);
	_renderTechnique.graphicsApiFilter()->setProfile(QGraphicsApiFilter::CoreProfile);
	_filterKey.setName(QStringLiteral("renderingStyle"));
	_filterKey.setValue(QStringLiteral("forward"));
	_renderTechnique.addFilterKey(&_filterKey);
	_effect.addTechnique(&_renderTechnique);

	_effect.addParameter(&_ambientParameter);
	_effect.addParameter(&_diffuseParameter);
	setEffect(&_effect);


}


Hix::Render::ModelMaterial::~ModelMaterial()
{
}

void Hix::Render::ModelMaterial::setDiffuse(const QColor& diffuse)
{
	_diffuseParameter.setValue(diffuse);
}

void Hix::Render::ModelMaterial::setAmbient(const QColor& ambient)
{
	_ambientParameter.setValue(ambient);

}

void Hix::Render::ModelMaterial::addParameter(const std::string& key)
{
	if (_additionalParameters.find(key) == _additionalParameters.end())
	{
		_additionalParameters.emplace(std::string(key), nullptr);
	}
}

void Hix::Render::ModelMaterial::removeParameter(const std::string& key)
{
	auto found = _additionalParameters.find(key);
	if (found != _additionalParameters.end())
	{
		_additionalParameters.erase(found);
	}
}

void Hix::Render::ModelMaterial::setParameterValue(const std::string& key, const QVariant& value)
{
	auto found = _additionalParameters.find(key);
	if (found != _additionalParameters.end())
	{
		found->second.setValue(value);
	}
}
