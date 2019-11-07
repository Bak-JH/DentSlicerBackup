#include "ModelMaterial.h"
#include <Qt3DRender>
#include <stdexcept>

#if  defined(QT_DEBUG) || defined(_DEBUG)
#include <stdexcept>
//enforce strict correctness
#define _MODEL_MATERIAL_STRICT
#endif


using namespace Qt3DRender;
using namespace Hix::Render;

//default
const QUrl VERT_URL = QUrl("qrc:/shaders/default.vert");

//flat frag shader
const QUrl FLAT_FRAG_URL = QUrl("qrc:/shaders/flat.frag");

//single color geom shader
const QUrl SINGLE_COLOR_GEOM_URL= QUrl("qrc:/shaders/singleColor.geom");

//per primitive geom shader
const QUrl PER_PRIMITIVE_GEOM_URL = QUrl("qrc:/shaders/perPrimitive.geom");

//layerview
const QUrl LAYERVIEW_FRAG_URL = QUrl("qrc:/shaders/layerview.frag");
const QUrl LAYERVIEW_GEOM_URL = QUrl("qrc:/shaders/layerview.geom");

Hix::Render::ModelMaterial::ModelMaterial():
	_ambientParameter(QStringLiteral("ambient"), QColor(70, 70, 70)),
	_diffuseParameter(QStringLiteral("diffuse"), QColor(140, 140, 140))
{

	//GLSL in this context can use following uniforms
	//Above declared parameters
	//QT defined uniforms, see QShaderProgram documentation

	_shaderProgram.setVertexShaderCode(QShaderProgram::loadSource(VERT_URL));
	changeMode(ShaderMode::SingleColor);
	_renderPass.setShaderProgram(&_shaderProgram);

	auto cullFace = new QCullFace();
	cullFace->setMode(QCullFace::CullingMode::Back);
	_renderPass.addRenderState(cullFace);

	auto depthTest = new QDepthTest();
	depthTest->setDepthFunction(QDepthTest::DepthFunction::Less);
	_renderPass.addRenderState(depthTest);
	_renderTechnique.addRenderPass(&_renderPass);
	_renderTechnique.graphicsApiFilter()->setApi(QGraphicsApiFilter::OpenGL);
	_renderTechnique.graphicsApiFilter()->setMajorVersion(4);
	_renderTechnique.graphicsApiFilter()->setMinorVersion(3);
	_renderTechnique.graphicsApiFilter()->setProfile(QGraphicsApiFilter::CoreProfile);
	_filterKey.setName(QStringLiteral("renderingStyle"));
	_filterKey.setValue(QStringLiteral("forward"));
	_renderTechnique.addFilterKey(&_filterKey);
	_effect.addTechnique(&_renderTechnique);
	//establish color look up table



	_singleColorParameter.setName(QStringLiteral("singleColor"));
	_singleColorParameter.setValue(QVector4D(0,0,0,0));

	_effect.addParameter(&_ambientParameter);
	_effect.addParameter(&_diffuseParameter);

	//coloring faces
	_effect.addParameter(&_singleColorParameter);
	setEffect(&_effect);
}


Hix::Render::ModelMaterial::~ModelMaterial()
{
	//qt sucks...
	_shaderProgram.setParent((QNode*)nullptr);
	_renderPass.setParent((QNode*)nullptr);
	_renderTechnique.setParent((QNode*)nullptr);
	_filterKey.setParent((QNode*)nullptr);
	_effect.removeParameter(&_ambientParameter);
	_effect.removeParameter(&_diffuseParameter);
	_effect.removeParameter(&_singleColorParameter);
	_effect.removeTechnique(&_renderTechnique);

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

void Hix::Render::ModelMaterial::addParameterWithKey(const std::string& key)
{
	if (_additionalParameters.find(key) == _additionalParameters.end())
	{
		auto result = _additionalParameters.emplace(std::string(key), nullptr);
		auto* ptr = &result.first->second;
		ptr->setName(QString::fromStdString(key));
		_effect.addParameter(ptr);
	}
}

void Hix::Render::ModelMaterial::removeParameterWithKey(const std::string& key)
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


void  Hix::Render::ModelMaterial::changeMode(ShaderMode mode) {
	if (_mode != mode)
	{
		//if layermode is now disabled
		if (_mode == ShaderMode::LayerMode)
		{
			removeParameterWithKey("height");
			removeParameterWithKey("fuckingStuipidWorldMatrix");
		}
		switch (mode) {
		case ShaderMode::SingleColor: // default
			_shaderProgram.setGeometryShaderCode(QShaderProgram::loadSource(SINGLE_COLOR_GEOM_URL));
			_shaderProgram.setFragmentShaderCode(QShaderProgram::loadSource(FLAT_FRAG_URL));
			break;
		case ShaderMode::PerPrimitiveColor:
			_shaderProgram.setGeometryShaderCode(QShaderProgram::loadSource(PER_PRIMITIVE_GEOM_URL));
			_shaderProgram.setFragmentShaderCode(QShaderProgram::loadSource(FLAT_FRAG_URL));
			break;
		case ShaderMode::LayerMode:
			_shaderProgram.setGeometryShaderCode(QShaderProgram::loadSource(LAYERVIEW_GEOM_URL));
			_shaderProgram.setFragmentShaderCode(QShaderProgram::loadSource(LAYERVIEW_FRAG_URL));
			addParameterWithKey("height");
			addParameterWithKey("fuckingStuipidWorldMatrix");

			break;
		default:
			break;
		}
		_mode = mode;
	}
}

ShaderMode Hix::Render::ModelMaterial::shaderMode() const
{
	return _mode;
}

bool test = true;
void Hix::Render::ModelMaterial::setColor(QVector4D color)
{
	qDebug() << _mode;

#ifdef _MODEL_MATERIAL_STRICT
	if (_mode != ShaderMode::SingleColor)
	{
		throw std::runtime_error("Attempted to set entire mesh material color when per primitive color is used");
	}
#endif

	_singleColorParameter.setValue(color);
}