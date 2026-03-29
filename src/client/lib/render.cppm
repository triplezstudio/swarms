
export module render.base;

import common;

export namespace render {

/**
 * The render interface.
 * It abstracts over any underlying render implementation, e.g. custom Vulkan, OpenGL
 * or any 3rd party render library.
 * In any case we want to be able to
 * a) switch render backends in the future if specific features are needed
 * b) be able to provide platform optimal render backends so we need to be able to support
 *    multiple renderers behind this interface anyway.
 */
class Renderer
{
  public:
  virtual void init() = 0;
  virtual void draw() = 0;
};

}