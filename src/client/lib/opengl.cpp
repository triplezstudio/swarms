
module;

#include <iostream>
#include <Eigen/Dense>
#ifdef _WIN32
#define NOMINMAX
#endif


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

