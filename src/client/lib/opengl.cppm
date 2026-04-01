module;
#include "defines.h"
#include <functional>
#include <Eigen/Dense>
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
      OpenGLRenderer(const OpenGLInitData& initData);
      void init() override;
      void beginDraw(render::PrimitiveType primitiveType) override;
      void endDraw() override;
      void emitPosition(Eigen::Vector3f pos) override;
      void emitColor(Eigen::Vector4f color) override;
      void emitUV(Eigen::Vector2f uv) override;
      void emitNormal(Eigen::Vector3f normal) override;

  private:
      OpenGLInitData initData;
};

}