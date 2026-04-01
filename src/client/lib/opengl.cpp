
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

render::OpenGLRenderer::OpenGLRenderer(const render::OpenGLInitData& initData)
  : initData(initData)
{

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

void render::OpenGLRenderer::init()
{

  // Create some basic default shaders:
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

}

void render::OpenGLShaderPipeline::link(std::vector<ShaderModule*> modules)
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

void render::OpenGLRenderer::endFrame()
{
  // Render the contents of our current immediate buffer:
  glBindVertexArray(immediatePerFrameVAO);
  //immediatePerFrameBuffer->bind();
  defaultShaderPipeline->bind();


  glDrawArrays(GL_TRIANGLES, 0, immediatePerFrameBuffer->getNumberOfElements());


}

void render::OpenGLRenderer::beginFrame()
{
  // Prepare for the next frame
  immediatePerFrameBuffer->clear();

  // TODO make clear color part of render API
  glClearColor(sinf(0.4), 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT);
}

void render::OpenGLRenderer::beginDraw(render::PrimitiveType primitiveType)
{
  positions.clear();
}
void render::OpenGLRenderer::endDraw()
{

  // Append every immediate vertex data stream into our perFrameBuffer.
  immediatePerFrameBuffer->appendData(positions);
}

void render::OpenGLRenderer::emitPosition(Eigen::Vector3f pos)
{
  positions.push_back(pos);
}

void render::OpenGLRenderer::emitColor(Eigen::Vector4f color)
{

}
void render::OpenGLRenderer::emitUV(Eigen::Vector2f uv)
{

}
void render::OpenGLRenderer::emitNormal(Eigen::Vector3f normal)
{

}

GLuint render::OpenGLRenderer::createVertexBuffer(render::VertexBufferCreateInfo vbCreateInfo) {
  GLuint vbo;
  glCreateBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  auto size =vbCreateInfo.data.size() * sizeof(float);
  glBufferData(GL_ARRAY_BUFFER, size , vbCreateInfo.data.data(), GL_DYNAMIC_DRAW);
  return vbo;
}


GLuint render::OpenGLRenderer::createVertexArrayObject()
{

  auto vao = (new VertexArrayObject::Builder())->positions({{0, 1, 2}})->build();
  return vao->getHandle();


}

void render::OpenGLShaderModule::init(render::ShaderType type, const std::string& source)
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

