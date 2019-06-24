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
	_specularParameter(QStringLiteral("specular"), QColor(0,0,0)),

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
	_filterKey.setParent(this);
	_filterKey.setName(QStringLiteral("renderingStyle"));
	_filterKey.setValue(QStringLiteral("forward"));
	_renderTechnique.addFilterKey(&_filterKey);
	_effect.addTechnique(&_renderTechnique);

	//create lights
	QVariantList positions = QVariantList();
	//positions in world coord
	positions << QVector3D(_lightDistance, _lightDistance, 0);
	positions << QVector3D(-_lightDistance, _lightDistance, 0);
	positions << QVector3D(_lightDistance, -_lightDistance, 0);
	positions << QVector3D(-_lightDistance, -_lightDistance, 0);
	_pointLightPositionsParameter.setName(QStringLiteral("pointLightPositions[0]"));


	_effect.addParameter(&_ambientParameter);
	_effect.addParameter(&_diffuseParameter);
	_effect.addParameter(&_specularParameter);
	_effect.addParameter(&_pointLightPositionsParameter);
	setEffect(&_effect);


	_pointLightPositionsParameter.setValue(positions);
	//this is hard coded in to shader to make the for loop in shader a const bound loop.
	//_pointLightCountParameter.setName(QStringLiteral("pointLightCount"));
	//_pointLightPositionsParameter.setValue(4);

	//since it's white color, no calculation is needed, so skipped for performance
	//_pointLightColorParameter.setName(QStringLiteral("pointLightColor"));
	//_pointLightColorParameter.setValue(QVector3D(1.0, 1.0, 1.0));

	//add parameters


}


Hix::Render::ModelMaterial::~ModelMaterial()
{
}
