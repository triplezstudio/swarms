
#include <Eigen/Dense>
export module render.base;

import common;


/**
 * The render interface.
 * It abstracts over any underlying render implementation, e.g. custom Vulkan, OpenGL
 * or any 3rd party render library.
 * In any case we want to be able to
 * a) switch render backends in the future if specific features are needed
 * b) be able to provide platform optimal render backends so we need to be able to support
 *    multiple renderers behind this interface anyway.
 */
export namespace render {

enum class PrimitiveType {
  Triangles,
  Lines,
  Quads

};

class Renderer
{
  public:
  virtual void init() = 0;
  virtual void beginDraw(PrimitiveType primitiveType) = 0;
  virtual void endDraw() = 0;
  virtual void emitPosition(Eigen::Vector3f position) = 0;
  virtual void emitColor(Eigen::Vector4f color) = 0;
  virtual void emitUV(Eigen::Vector2f uv) = 0;
  virtual void emitNormal(Eigen::Vector3f normal) = 0;
};

}