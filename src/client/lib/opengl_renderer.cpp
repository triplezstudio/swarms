
module;
#include <iostream>
#include <Eigen/Dense>
#ifdef _WIN32
#define NOMINMAX
#endif

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <algorithm>
#include <limits>

module render.opengl;

import common;
import windowing;


namespace tz
{

void OpenGLRenderer::init(Window* window)
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
  immediatePerFrameBuffer = new OpenGLVertexBuffer(1024 * 16);
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

void OpenGLRenderer::clearScreen()
{
  //glClearColorf(1, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGLShaderPipeline::link(const std::vector<ShaderModule*>& modules)
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

WindowDesc OpenGLRenderer::getRequiredWindowDesc()
{
  WindowDesc wd;
  wd.api = tz::WindowDesc::GraphicsAPI::OpenGL;
  wd.width = 640;
  wd.height = 480;

  return wd;
}

GLenum OpenGLRenderer::primitiveTypeToEnum(PrimitiveType pt)
{
  switch (pt)
  {
    case PrimitiveType::Triangles: return GL_TRIANGLES;
    case PrimitiveType::Lines: return GL_LINES;
    case PrimitiveType::Quads: return GL_TRIANGLES;
    default: return GL_TRIANGLES;
  }
}

void OpenGLRenderer::execCmdDraw(CmdDraw* cmd)
{
  auto vao = vaoCache[currentPSO->vertexLayout.toHash()];
  glBindVertexArray(vao);

  glDrawArrays(primitiveTypeToEnum(currentPSO->renderState.primitiveType), cmd->firstVertex, cmd->vertexCount);
}

void OpenGLRenderer::execCmdBindVertexBuffers(CmdBindVertexBuffers* cmd)
{

  auto vao = vaoCache[currentPSO->vertexLayout.toHash()];
  for (auto& b : currentPSO->vertexLayout.bindings)
  {
    auto vb = cmd->vertexBuffers[b.bufferSlot];
    glVertexArrayVertexBuffer(vao,
                              b.bufferSlot,
                              *reinterpret_cast<GLuint*>(vb->getHandle()),
                              0,
                              b.stride);
  }

}

VertexBuffer *OpenGLRenderer::createVertexBuffer(const std::vector<Eigen::Vector3f> &data)
{
  auto vb = new OpenGLVertexBuffer(data);
  return vb;
}

void OpenGLRenderer::execCmdBindPipeline(CmdBindPipeline* cmd)
{
  // Bind the current shader program:
  auto prog =*reinterpret_cast<GLuint*>(cmd->pso->shaderPipeline->getHandle());
  glUseProgram(prog);

  // Apply all attributes of the current render state:
  // TODO: cache/make more efficient.
  //       We do not want to set states which are already active on GL.
  cmd->pso->renderState.stencilTesting ? glEnable(GL_STENCIL_TEST) : glDisable(GL_STENCIL_TEST);
  cmd->pso->renderState.depthTesting ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);

  switch (cmd->pso->renderState.cullMode)
  {
    case CullMode::Back:
      glEnable(GL_CULL_FACE);
      glCullFace(GL_BACK); break;
    case CullMode::Front:
      glEnable(GL_CULL_FACE);
      glCullFace(GL_FRONT); break;
    case CullMode::None:
      glDisable(GL_CULL_FACE); break;

  }


  switch (cmd->pso->renderState.frontFace)
  {
    case FrontFace::Clockwise:
      glFrontFace(GL_CW); break;
    case FrontFace::CounterClockwise:
      glFrontFace(GL_CCW); break;
  }

  switch (cmd->pso->renderState.fillMode)
  {
    case FillMode::Solid:
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); break;
    case FillMode::Line:
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); break;
  }

  if (cmd->pso->renderState.blending)
  {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);
  } else {
    glDisable(GL_BLEND);
  }

  // These must be bound to a VAO.
  // VAOs are cached by the incoming vertex layout.
  if (vaoCache.find(cmd->pso->vertexLayout.toHash()) == vaoCache.end()) {
    GLuint newVAO;
    glCreateVertexArrays(1, &newVAO);
    vaoCache[cmd->pso->vertexLayout.toHash()] = newVAO;
  }
  auto vao = vaoCache[cmd->pso->vertexLayout.toHash()];

  for (auto & attr : cmd->pso->vertexLayout.attributes) {
    glEnableVertexArrayAttrib(vao, attr.shaderLocation);
    glVertexArrayAttribFormat(vao, attr.shaderLocation,
                              attr.componentCount,
                              GL_FLOAT, GL_FALSE, attr.offset);
    glVertexArrayAttribBinding(vao, attr.shaderLocation, attr.bufferSlot);
  }

  for (auto& binding : cmd->pso->vertexLayout.bindings)
  {
      if (binding.vertexInputRate == VertexInputRate::PerInstance)
      {
        glVertexBindingDivisor(binding.bufferSlot, 1);
      }

  }

  // Due to the nature of GL we can not pre-set
  // the glVertexAttribPointer which would
  // come from the PSO's vertex layout, so we must
  // store the current PSO to be used during later commands (e.g. draw).
  currentPSO = cmd->pso;

}

void OpenGLRenderer::executeCommandBuffer(tz::CommandBuffer *commandBuffer)
{
  for (auto cmd : *commandBuffer) {
    if (auto c = dynamic_cast<CmdBindPipeline*>(cmd))
    {
      execCmdBindPipeline(c);

    }
    else if (auto c = dynamic_cast<CmdBindVertexBuffers*>(cmd))
    {
      execCmdBindVertexBuffers(c);
    }
    else if (auto c = dynamic_cast<CmdDraw*>(cmd))
    {
      execCmdDraw(c);
    }


  }
}

void OpenGLRenderer::submitCommandBuffer(tz::CommandBuffer *commandBuffer)
{
  frameCommandBuffers.push_back(commandBuffer);
}


void OpenGLRenderer::executeImmediateCommands()
{
  // Render the contents of our current immediate buffer:
  glBindVertexArray(immediatePerFrameVAO);
  //immediatePerFrameBuffer->bind();
  defaultShaderPipeline->bind();
  glDrawArrays(GL_TRIANGLES, 0, immediatePerFrameBuffer->getNumberOfElements());

}

void OpenGLRenderer::executeCommandBuffers()
{
  for (auto& cb: frameCommandBuffers)
  {
    executeCommandBuffer(cb);
  }
}

void OpenGLRenderer::endFrame()
{
  executeImmediateCommands();
  executeCommandBuffers();

}

void OpenGLRenderer::beginFrame()
{
  // Prepare for the next frame
  immediatePerFrameBuffer->clear();

  frameCommandBuffers.clear();

  // TODO make clear color part of render API
  glClearColor(sinf(0.4), 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLRenderer::beginDraw(tz::PrimitiveType primitiveType)
{
  positions.clear();
}
void OpenGLRenderer::endDraw()
{

  // Append every immediate vertex data stream into our perFrameBuffer.
  immediatePerFrameBuffer->appendData(positions);
}

void OpenGLRenderer::emitPosition(Eigen::Vector3f pos)
{
  positions.push_back(pos);
}

void OpenGLRenderer::emitColor(Eigen::Vector4f color)
{

}
void OpenGLRenderer::emitUV(Eigen::Vector2f uv)
{

}
void OpenGLRenderer::emitNormal(Eigen::Vector3f normal)
{

}

GLuint OpenGLRenderer::createVertexBuffer(VertexBufferCreateInfo vbCreateInfo) {
  GLuint vbo;
  glCreateBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  auto size =vbCreateInfo.data.size() * sizeof(float);
  glBufferData(GL_ARRAY_BUFFER, size , vbCreateInfo.data.data(), GL_DYNAMIC_DRAW);
  return vbo;
}


GLuint OpenGLRenderer::createVertexArrayObject()
{
  auto vao = (new OpenGLVertexArrayObject::Builder())->positions({{0, 1, 2}})->build();
  return vao->getHandle();
}


void OpenGLShaderModule::init(ShaderType type, const std::string& source)
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
}



