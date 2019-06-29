#include "ModelMaterial.h"
#include <Qt3DRender>
#include <stdexcept>
using namespace Qt3DRender;
using namespace Hix::Render;
const QUrl VERT_URL = QUrl("qrc:/shaders/model_shader.vert");
const QUrl GEOM_URL = QUrl("qrc:/shaders/model_shader.geom");
const QUrl FRAG_URL = QUrl("qrc:/shaders/model_shader.frag");
const QUrl FRAG_LAYERVIEW_URL = QUrl("qrc:/shaders/layerview_shader.frag");

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


	_shaderProgram.setVertexShaderCode(QShaderProgram::loadSource(VERT_URL));
	_shaderProgram.setGeometryShaderCode(QShaderProgram::loadSource(GEOM_URL));
	_shaderProgram.setFragmentShaderCode(QShaderProgram::loadSource(FRAG_URL));
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
	for (auto& each : _additionalParameters)
	{
		auto ptr = &each.second;
		_effect.removeParameter(ptr);
	}
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
		auto result = _additionalParameters.emplace(std::string(key), nullptr);
		auto* ptr = &result.first->second;
		ptr->setName(QString::fromStdString(key));
		_effect.addParameter(ptr);
	}
}

void Hix::Render::ModelMaterial::removeParameter(const std::string& key)
{
	auto found = _additionalParameters.find(key);
	if (found != _additionalParameters.end())
	{
		auto ptr = &found->second;
		_effect.removeParameter(ptr);
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


void  Hix::Render::ModelMaterial::changeColor(ModelColor mode) {
	if (colorMode != mode)
	{
		//if layermode is now disabled
		if (colorMode == ModelColor::LayerMode)
		{
			_shaderProgram.setFragmentShaderCode(QShaderProgram::loadSource(FRAG_URL));
			removeParameter("height");
		}
		switch (mode) {
		case ModelColor::Default: // default
			setAmbient(QColor(65, 65, 70));
			setDiffuse(QColor(97, 185, 192));
			break;
		case ModelColor::Selected:
			setAmbient(QColor(115, 115, 115));
			setDiffuse(QColor(130, 208, 125));
			break;
		case ModelColor::OutOfBound:
			setAmbient(QColor(0, 0, 0));
			setDiffuse(QColor(0, 0, 0));
			break;
		case ModelColor::LayerMode:
			_shaderProgram.setFragmentShaderCode(QShaderProgram::loadSource(FRAG_LAYERVIEW_URL));
			addParameter("height");
			setAmbient(QColor(115, 115, 115));
			setDiffuse(QColor(130, 208, 125));
			break;
		default:
			break;
		}
		colorMode = mode;
	}
}