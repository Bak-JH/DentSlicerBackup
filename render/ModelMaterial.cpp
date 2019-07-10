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
	_ambientParameter(QStringLiteral("ambient"), QColor(50, 50, 50)),
	_diffuseParameter(QStringLiteral("diffuse"), QColor(140, 140, 120))
{

	//GLSL in this context can use following uniforms
	//Above declared parameters
	//QT defined uniforms, see QShaderProgram documentation

	_shaderProgram.setVertexShaderCode(QShaderProgram::loadSource(VERT_URL));
	changeMode(ShaderMode::SingleColor);
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
	//establish color look up table

	QVariantList colors = QVariantList();
	//positions in world coord
	//enum MeshColor
	//{
	//	Default = 0,
	//	Selected = 1,
	//	SelectedFace = 2,
	//	Support = 3,
	//	Raft = 4
	//};
	colors << Hix::Render::Colors::Default;
	colors << Hix::Render::Colors::Selected;
	colors << Hix::Render::Colors::SelectedFace;
	colors << Hix::Render::Colors::Support;
	colors << Hix::Render::Colors::Raft;
	QVariantList TTT = QVariantList();
	uint testInt = 1u;
	TTT << testInt;
	TTT << testInt;
	TTT << testInt;
	TTT << testInt;
	TTT << testInt;
	TTT << testInt;

	QVariant v(1);




	//, primitiveColorBuffer(Qt3DRender::QBuffer::ShaderStorageBuffer, this)

	Qt3DRender::QBuffer* buff = new Qt3DRender::QBuffer(this);
	buff->setType(Qt3DRender::QBuffer::BufferType::ShaderStorageBuffer);

	QByteArray testData;
	testData.resize(4 * sizeof(uint));
	uint* rawUintArray = reinterpret_cast<uint*>(testData.data());
	size_t idx = 0;
	for (uint i = 0; i < 4; ++i)
	{
		rawUintArray[i] = 2u;
	}
	buff->setData(testData);
	//_perPrimitiveColorParameter.setValue(QVariant::fromValue(buffer.id()));
	//_perPrimitiveColorParameter.setName("perPrimitiveColorCode");
	//_effect.addParameter(&_perPrimitiveColorParameter);
	//addParameter(new Qt3DRender::QParameter("perPrimitiveColorCode", QVariant::fromValue(buffer.data()), this));

	auto param = new QParameter(this);
	param->setName(QStringLiteral("input"));
	param->setValue(QVariant::fromValue(buff->id()));

	_renderPass.addParameter(param);





	//_perPrimitiveColorParameter.setName(QStringLiteral("perPrimitiveColorCode[0]"));
	//_perPrimitiveColorParameter.setValue(TTT);
	_colorTableParameter.setName(QStringLiteral("colorTable[0]"));
	_colorTableParameter.setValue(colors);
	_singleColorParameter.setName(QStringLiteral("singleColor"));
	_singleColorParameter.setValue(QVector3D(0,0,0));

	_effect.addParameter(&_ambientParameter);
	_effect.addParameter(&_diffuseParameter);

	//coloring faces
	_effect.addParameter(&_colorTableParameter);
	//_effect.addParameter(&_perPrimitiveColorParameter);
	_effect.addParameter(&_singleColorParameter);


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

void Hix::Render::ModelMaterial::setPerPrimitiveColorSSBO(Qt3DRender::QBuffer& buffer)
{
	QByteArray testData;
	testData.resize(4 * sizeof(uint));
	uint* rawUintArray = reinterpret_cast<uint*>(testData.data());
	size_t idx = 0;
	for (uint i = 0; i < 4; ++i)
	{
		rawUintArray[i] = 2;
	}
	buffer.setData(testData);
	//_perPrimitiveColorParameter.setValue(QVariant::fromValue(buffer.id()));
	//_perPrimitiveColorParameter.setName("perPrimitiveColorCode");
	//_effect.addParameter(&_perPrimitiveColorParameter);
	addParameter(new Qt3DRender::QParameter("perPrimitiveColorCode", QVariant::fromValue(buffer.data()), this));



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

void Hix::Render::ModelMaterial::setColor(QVector3D color)
{
#ifdef _MODEL_MATERIAL_STRICT
	if (_mode != ShaderMode::SingleColor)
	{
		throw std::runtime_error("Attempted to set entire mesh material color when per primitive color is used");
	}
#endif
	_singleColorParameter.setValue(color);
}

void Hix::Render::ModelMaterial::setColorCodes(QVariantList& colorCodes)
{

#ifdef _MODEL_MATERIAL_STRICT
	if (_mode == ShaderMode::SingleColor)
	{
		throw std::runtime_error("Attempted to set per primitive color when singular color is used for entire material");
	}
#endif
	//_perPrimitiveColorParameter.setValue(colorCodes);
}
