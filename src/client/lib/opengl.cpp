
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

}

void render::OpenGLRenderer::init()
{

}

void render::OpenGLRenderer::beginDraw(render::PrimitiveType primitiveType)
{

}
void render::OpenGLRenderer::endDraw()
{

}

void render::OpenGLRenderer::emitPosition(Eigen::Vector3f pos)
{

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

