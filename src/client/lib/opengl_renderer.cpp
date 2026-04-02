
module;

#include <iostream>
#include <Eigen/Dense>
#ifdef _WIN32
#define NOMINMAX
#endif


#include <GL/glew.h>
#include <SDL_syswm.h>
#include <SDL2/SDL.h>
#include <algorithm>
#include <limits>
#include <fstream>

module render.opengl;

import common;



void tz::OpenGLRenderer::init(tz::Window* window)
{

  this->window = window;

  // Initialize GLEW for function loading
  // TODO only do this on windows?
  GLenum err = glewInit();
  if (GLEW_OK != err)
  {
    // GLEW initialization failed
    fprintf(stderr, "Error initializing GLEW: %s\n", glewGetErrorString(err));
    exit(1);
  }

  // Create default shaders:
  auto vsSource = R"(
    #version 460 core
    layout(location = 0) in vec3 pos;
    void main() {
        gl_Position = vec4(pos, 1);
    }
  )";

  auto fsSource = R"(
    #version 460 core

    out vec4 color;
    void main() {
      color = vec4(1, 1, 1, 1);

    }
  )";

  auto defaultVertexShader =  new OpenGLShaderModule();
  defaultVertexShader->init(ShaderType::Vertex, vsSource);
  auto defaultFragmentShader =  new OpenGLShaderModule();
  defaultFragmentShader->init(ShaderType::Fragment, fsSource);
  defaultShaderPipeline = new OpenGLShaderPipeline();
  defaultShaderPipeline->link({defaultVertexShader, defaultFragmentShader});

  // Prepare per-frame buffer to store the immediate-command data into:
  immediatePerFrameVAO = createVertexArrayObject();
  glBindVertexArray(immediatePerFrameVAO);
  immediatePerFrameBuffer = new VertexBuffer(1024 * 16);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(
    0,
    3,
    GL_FLOAT,
    GL_FALSE,
    sizeof(float) * 3,
    (void *) 0
  );
  immediatePerFrameBuffer->map();

}

void tz::OpenGLShaderPipeline::link(std::vector<ShaderModule*> modules)
{
  programHandle = glCreateProgram();
  for (auto& m : modules) {
    auto sh = m->getHandle();
    glAttachShader(programHandle, *reinterpret_cast<GLuint*>(sh));
  }

  glLinkProgram(programHandle);

  GLint linkStatus;
  glGetProgramiv(programHandle, GL_LINK_STATUS, &linkStatus);

  if (GL_FALSE == linkStatus) {
    std::cerr << "Error during shader linking" << std::endl;
    GLint maxLength = 0;
    glGetProgramiv(programHandle, GL_INFO_LOG_LENGTH, &maxLength);
    std::vector<GLchar> infoLog(maxLength);
    glGetProgramInfoLog(programHandle, maxLength, &maxLength, &infoLog[0]);
    std::cerr << infoLog.data() << std::endl;
    glDeleteProgram(programHandle);
    throw std::runtime_error(infoLog.data());
  }

}

tz::WindowDesc tz::OpenGLRenderer::getRequiredWindowDesc()
{
  WindowDesc wd;
  wd.api = tz::WindowDesc::GraphicsAPI::OpenGL;
  wd.width = 640;
  wd.height = 480;

  return wd;
}

void tz::OpenGLRenderer::endFrame()
{
  // Render the contents of our current immediate buffer:
  glBindVertexArray(immediatePerFrameVAO);
  //immediatePerFrameBuffer->bind();
  defaultShaderPipeline->bind();


  glDrawArrays(GL_TRIANGLES, 0, immediatePerFrameBuffer->getNumberOfElements());


}

void tz::OpenGLRenderer::beginFrame()
{
  // Prepare for the next frame
  immediatePerFrameBuffer->clear();

  // TODO make clear color part of render API
  glClearColor(sinf(0.4), 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT);
}

void tz::OpenGLRenderer::beginDraw(tz::PrimitiveType primitiveType)
{
  positions.clear();
}
void tz::OpenGLRenderer::endDraw()
{

  // Append every immediate vertex data stream into our perFrameBuffer.
  immediatePerFrameBuffer->appendData(positions);
}

void tz::OpenGLRenderer::emitPosition(Eigen::Vector3f pos)
{
  positions.push_back(pos);
}

void tz::OpenGLRenderer::emitColor(Eigen::Vector4f color)
{

}
void tz::OpenGLRenderer::emitUV(Eigen::Vector2f uv)
{

}
void tz::OpenGLRenderer::emitNormal(Eigen::Vector3f normal)
{

}

GLuint tz::OpenGLRenderer::createVertexBuffer(tz::VertexBufferCreateInfo vbCreateInfo) {
  GLuint vbo;
  glCreateBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  auto size =vbCreateInfo.data.size() * sizeof(float);
  glBufferData(GL_ARRAY_BUFFER, size , vbCreateInfo.data.data(), GL_DYNAMIC_DRAW);
  return vbo;
}


GLuint tz::OpenGLRenderer::createVertexArrayObject()
{

  auto vao = (new VertexArrayObject::Builder())->positions({{0, 1, 2}})->build();
  return vao->getHandle();


}

void tz::OpenGLShaderModule::init(tz::ShaderType type, const std::string& source)
{

  switch (type)
  {
    case ShaderType::Vertex: handle = glCreateShader(GL_VERTEX_SHADER); break;
    case ShaderType::Fragment: handle = glCreateShader(GL_FRAGMENT_SHADER); break;

  }

  auto sourceChar = source.c_str();
  glShaderSource(handle, 1, &sourceChar, NULL);
  glCompileShader(handle);

  GLint compileStatus;
  glGetShaderiv(handle, GL_COMPILE_STATUS, &compileStatus);
  if (GL_FALSE == compileStatus) {
    GLint logSize = 0;
    glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &logSize);
    std::vector<GLchar> errorLog(logSize);
    glGetShaderInfoLog(handle, logSize, &logSize, &errorLog[0]);

    std::cout << "fragment shader error: " << std::string(errorLog.data());
    glDeleteShader(handle);
    throw std::runtime_error(errorLog.data());

  }


}

