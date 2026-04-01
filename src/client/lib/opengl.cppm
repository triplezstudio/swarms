module;
#include "defines.h"
#include <functional>
#include <optional>

export module render.opengl;

import render.base;

import common;

export namespace render {

struct OpenGLInitData
{
  client_common::NativeHandles nativeHandles;
  std::function<void(int *width, int *height)> displaySizeFunc;
};

/**
 * OpenGLRenderer is a custom renderer using the OpenGL 4.6 API.
 *
 */
class SWARMS_API OpenGLRenderer : public render::Renderer
{
  public:
      OpenGLRenderer();
      void init() override;
      void beginDraw(render::PrimitiveType primitiveType) override;
      void endDraw() override;
      void emitPosition(Eigen::Vector3f pos) override;
};

}