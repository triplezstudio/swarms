
export module render.base;

import common;

export namespace render {

class Renderer
{
  public:
  virtual void init(client_common::NativeHandleProvider* nativeWindowProvider) = 0;
  virtual void draw() = 0;
};

}