#include "OpenGLIncludes.h"

#include "gpuSlicer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>



#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>
#include <limits>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <execution>
#include <thread>
#include <qurl.h>
#include <qfile.h>
#include "include/shader.h"
//#include "vertex.h"
//#include "util.h"

#define STBI_WINDOWS_UTF16
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "include/stb_image_write.h"


using GLlimit =  std::numeric_limits<GLfloat>;

using namespace Hix::Slicer;
// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

static void glfwErrorCallback(int, const char* message) {
    fprintf(stderr, "%s\n", message);
}

static void glMessageCallback(GLenum, GLenum, GLuint, GLenum, GLsizei,
    const GLchar* message, const void*) {
    glfwErrorCallback(0, message);
}



void Hix::Slicer::SlicerGL::setUniforms(Shader& shader, float height, float maxBright)
{

    //glm::mat4 proj = glm::ortho(-_imgX, _imgX, -_imgY, _imgY, -total_thickness, total_thickness);
    glm::mat4 proj = glm::ortho(-(float)_imgX, (float)_imgX, -(float)_imgY, (float)_imgY,(float) -GLlimit::epsilon(), (float)(_bounds.zMax() + GLlimit::epsilon()));
    glm::mat4 view = glm::lookAt(
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, -1.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, std::min(0.0f, -height)));
    glm::mat4 mvp = proj * view * model; // Remember, matrix multiplication is the other way around
    shader.setMat4("mvp", mvp);
    shader.setFloat("maxBright", maxBright);


}

void Hix::Slicer::SlicerGL::writeToFile(const std::vector<uint8_t>& data, size_t index)
{
    std::stringstream idxStream;
    idxStream << index;
    auto idxStr = idxStream.str() + ".png";
    std::filesystem::path file = _outPath/(idxStr);
    stbi_write_png(file.c_str(), _resX, _resY, 1, data.data(), _resX);
    _fileWriteSem.notify();
}



void Hix::Slicer::SlicerGL::prepareSlice()
{
    glGenFramebuffers(1, &_sliceFBO);
    glGenTextures(1, &_sliceTex);
    glGenRenderbuffers(1, &_sliceBuf);

    glBindTexture(GL_TEXTURE_2D, _sliceTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _resX, _resY, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}


struct plus_overflow {
    auto operator()(const uint8_t& a, const uint8_t& b) const {
        if ((uint16_t)a + (uint16_t)b > 255)
            return 255;
        return a + b;
    }
};

void Hix::Slicer::SlicerGL::glfwSlice(Shader& shader, float height, size_t index)
{
    auto bufferIndex = index % _concurrentWriteMax;
    std::fill(_finalBuffers[bufferIndex].begin(), _finalBuffers[bufferIndex].end(), 0);
    GLfloat currentAAZ = height - _layer;
    GLfloat brightnessPerSample = 1.0f / (GLfloat)_sampleZ;
    GLfloat SSZ = _layer / (GLfloat)_sampleZ / 2.0;
    for (size_t i = 0; i < _sampleZ; ++i)
    {
        currentAAZ += SSZ;
        glEnable(GL_STENCIL_TEST);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        glBindVertexArray(_VAO);
        shader.use();
        setUniforms(shader, currentAAZ, brightnessPerSample);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        glStencilFunc(GL_ALWAYS, 0, 0xFF);
        glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
        glDrawArrays(GL_TRIANGLES, 0, _vertCnt);

        glCullFace(GL_BACK);
        glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
        glDrawArrays(GL_TRIANGLES, 0, _vertCnt);
        glDisable(GL_CULL_FACE);

        glClear(GL_COLOR_BUFFER_BIT);
        glBindVertexArray(_maskVAO);
        glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
        glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glDisable(GL_STENCIL_TEST);
        glReadPixels(0, 0, _resX, _resY, GL_RED, GL_UNSIGNED_BYTE, _singlePassBuffer.data());
        //std::transform(std::execution::seq, _finalBuffers[bufferIndex].begin(), _finalBuffers[bufferIndex].end(), _singlePassBuffer.begin(), _finalBuffers[bufferIndex].begin(), plus_overflow());
        std::transform(std::execution::par_unseq, _finalBuffers[bufferIndex].begin(), _finalBuffers[bufferIndex].end(), _singlePassBuffer.begin(), _finalBuffers[bufferIndex].begin(), plus_overflow());

        glfwSwapBuffers(_window);
        glfwPollEvents();
    }
    //do not proceed when all cores are busy writing
    _fileWriteSem.wait();
    _pendingWrites.emplace_back(std::async(std::launch::async, &SlicerGL::writeToFile, this,  _finalBuffers[bufferIndex], index));
}



Hix::Slicer::SlicerGL::SlicerGL(float delta, std::filesystem::path outPath, size_t sampleXY, size_t sampleZ):
    _concurrentWriteMax(std::thread::hardware_concurrency()),
    _fileWriteSem(_concurrentWriteMax),
    _outPath(outPath),
    _layer(delta),
    _sampleXY(sampleXY),
    _sampleZ(sampleZ)
{

}

void Hix::Slicer::SlicerGL::setScreen(float pixelWidth, size_t imgX, size_t imgY)
{
    _pixel = pixelWidth;
    _resX = imgX;
    _resY = imgY;

    _imgX = _resX * _pixel / 2.0f;
    _imgY = _resY * _pixel / 2.0f;
    if (!glfwInit())
    {
        throw std::runtime_error("GLFW Init failed");
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //for headless slicing
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    if (_sampleXY > 1)
    {
        glfwWindowHint(GLFW_SAMPLES, _sampleXY);
    }

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif
    //enable debug if needed
#ifdef _DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    glfwSetErrorCallback(glfwErrorCallback);
#endif

    // set the window's display mode
    _window = glfwCreateWindow(_resX, _resY, "STL viewer", NULL, NULL);

    if (!_window)
    {
        glfwTerminate();
        throw std::runtime_error("create window failed");
    }

    // make the windows context current
    glfwMakeContextCurrent(_window);
    glfwSetFramebufferSizeCallback(_window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        throw std::runtime_error("glad failed");
    }
    //std::cout << "gl version: " << glGetString(GL_VERSION) << std::endl;

    //_ogl.reset(new QOpenGLFunctions());

    //glewExperimental = GL_TRUE;
    //GLenum err = glewInit();
    //if (err != GLEW_OK) {
    //    glfwTerminate();
    //    throw std::runtime_error("glew failed");
    //}
#ifdef _DEBUG
    //glDebugMessageCallback(glMessageCallback, NULL);
#endif

    glViewport(0, 0, _resX, _resY);
    glEnable(GL_MULTISAMPLE);

    //for the sake of performance, all memories used in this operation are allocated once, here.
    _singlePassBuffer.assign(_resX* _resY, 0);
    _finalBuffers.assign(_concurrentWriteMax, {});
    for (auto& each : _finalBuffers)
    {
        each.assign(_resX* _resY, 0);
    }
}

void Hix::Slicer::SlicerGL::setBounds(const Hix::Engine3D::Bounds3D& bound)
{
    _bounds = bound;
}



Hix::Slicer::SlicerGL::~SlicerGL()
{
}

void Hix::Slicer::SlicerGL::addVtcs(const std::vector<float>& vtcs)
{
    _vertCnt = vtcs.size();
    glGenVertexArrays(1, &_VAO);
    glGenBuffers(1, &_vertVBO);
    glBindVertexArray(_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, _vertVBO);
    glBufferData(GL_ARRAY_BUFFER, _vertCnt * sizeof(GLfloat), vtcs.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    GLfloat maskVert[] = {
    -_imgX, -_imgY, 0.0f,
    _imgX, -_imgY, 0.0f,
    _imgX, _imgY, 0.0f,

    -_imgX, -_imgY, 0.0f,
    _imgX, _imgY, 0.0f,
    -_imgX, _imgY, 0.0f,
    };


    glGenVertexArrays(1, &_maskVAO);
    glGenBuffers(1, &_maskVBO);
    glBindVertexArray(_maskVAO);
    glBindBuffer(GL_ARRAY_BUFFER, _maskVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(maskVert), &maskVert, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

}


size_t Hix::Slicer::SlicerGL::run()
{
    //shitty qt code below
    QFile vertFile(":/shaders/slice.vert");
    QFile fragFile(":/shaders/slice.frag");

    if (!vertFile.open(QIODevice::ReadOnly)|| !fragFile.open(QIODevice::ReadOnly)) {
        throw std::runtime_error("failed to load shader files");
    }
    auto vertStr = vertFile.readAll().toStdString();
    auto fragStr = fragFile.readAll().toStdString();

    //auto test = FRAG.toLocalFile();
    Shader sliceShader(vertStr, fragStr, std::string());

    prepareSlice();
    size_t i = 0;
    GLfloat height = GLlimit::epsilon() * 5; // slice height

    while (height < _bounds.zMax() - GLlimit::epsilon())
    {
        // render slice in printer's full resolution
        glfwSlice(sliceShader, height, i);
        height += _layer;
        ++i;
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &_VAO);
    glDeleteBuffers(1, &_vertVBO);
    glDeleteVertexArrays(1, &_maskVAO);
    glDeleteBuffers(1, &_maskVBO);

    glfwTerminate();
    for (auto& t : _pendingWrites)
    {
        t.wait();
    }
    return i;
}
